-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2025 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Jakub Dorňák <xdorna06@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_INV  : out std_logic;                      -- pozadavek na aktivaci inverzniho zobrazeni (1)
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
    --CNT
    signal CNT : std_logic_vector(7 downto 0); 
    signal CNT_INC  : std_logic;
    signal CNT_DEC  : std_logic;

    -- PTR
    signal PTR      : std_logic_vector(12 downto 0);
    signal PTR_INC  : std_logic;
    signal PTR_DEC  : std_logic;

    -- PC
    signal PC       : std_logic_vector(12 downto 0);
    signal PC_INC   : std_logic;
    signal PC_DEC   : std_logic;
    signal PTR_RST  : std_logic;

    -- MX1
    signal MUX1_SEL : std_logic;

    --MX2
    signal MUX2_SEL : std_logic_vector(1 downto 0);

    --DATA HEX
    signal DATA_HEX : std_logic_vector(7 downto 0);

    --FSM
    type fsm_state is (
        s_start,
        s_fetch_ptr,
        s_fetch_pc,
        s_init,
        s_decode,
        s_increment_value,
        s_decrement_value,
        s_print,
        s_write_hex,
        s_while_end,
        s_while_find_start,
        s_while_pc_fetch,
        s_while_check_start,
        s_while_skip_fetch,
        s_while_skip_check,
        s_do_end,
        s_do_pc_fetch,
        s_do_find_start,
        s_halt
    );
    signal curr_state   : fsm_state;
    signal next_state   : fsm_state;
begin
    --CNT_C--
    cnt_c: process (CLK, RESET) is begin
        if RESET = '1' then
            CNT <= (others => '0');
        elsif (CLK'event) and (CLK = '1') then
            if CNT_INC = '1' then
                if CNT = X"FF" then
                    CNT <= (others => '0');
                else
                    CNT <= CNT + 1;
                end if;
            elsif CNT_DEC = '1' then
                if CNT = X"00" then
                    CNT <= (others => '1');
                else
                    CNT <= CNT - 1;
                end if;
            end if;
        end if;
    end process;
    --CNT_C--

    --PC_C--
    pc_c: process (CLK, RESET) is begin
        if RESET = '1' then
            PC <= (others => '0');
        elsif (CLK'event) and (CLK = '1') then
            if PC_INC = '1' then
                if PC = "1111111111111" then
                    PC <= (others => '0');
                else
                    PC <= PC + 1;
                end if;
            elsif PC_DEC = '1' then
                if PC = "0000000000000" then
                    PC <= (others => '1');
                else
                    PC <= PC - 1;
                end if;
            end if;
        end if;
    end process;
    --PC_C--


    --PTR_C--
    ptr_c: process (CLK, RESET) is begin
        if RESET = '1' then
            PTR <= (others => '0');
        elsif (CLK'event) and (CLK = '1') then
            if PTR_INC = '1' then
                if PTR = "1111111111111" then
                    PTR <= (others => '0');
                else
                    PTR <= PTR + 1;
                end if;
            elsif PTR_DEC = '1' then
                if PTR = "0000000000000" then
                    PTR <= (others => '1');
                else
                    PTR <= PTR - 1;
                end if;
            end if;
        end if;
    end process;
    --PTR_C--

    --MUX1_C--
    DATA_ADDR <= PC when MUX1_SEL = '0' else PTR;
    --MUX1_C--

    --MUX2_C--
    with MUX2_SEL select
        DATA_WDATA <= DATA_HEX when "00",
                    DATA_RDATA + 1 when "01",
                    DATA_RDATA - 1 when "10",
                    IN_DATA when "11",
                    DATA_RDATA when others;
    --MUX2_C--

    --HEX_CODER--
    hex_c: process (CLK, RESET) 
    begin
        if RESET = '1' then
            DATA_HEX <= (others => '0');
        elsif (CLK'event) and (CLK = '1') then
            case DATA_RDATA is
                when X"30" => DATA_HEX <= X"00";
                when X"31" => DATA_HEX <= X"10";
                when X"32" => DATA_HEX <= X"20";
                when X"33" => DATA_HEX <= X"30";
                when X"34" => DATA_HEX <= X"40";
                when X"35" => DATA_HEX <= X"50";
                when X"36" => DATA_HEX <= X"60";
                when X"37" => DATA_HEX <= X"70";
                when X"38" => DATA_HEX <= X"80";
                when X"39" => DATA_HEX <= X"90";
                when X"41" => DATA_HEX <= X"A0";
                when X"42" => DATA_HEX <= X"B0";
                when X"43" => DATA_HEX <= X"C0";
                when X"44" => DATA_HEX <= X"D0";
                when X"45" => DATA_HEX <= X"E0";
                when X"46" => DATA_HEX <= X"F0";
                when others => DATA_HEX <= X"00";
            end case;
        end if;
    end process;
    --HEX_CODER--

    --FSM state sync--
    fsm_sync : process(CLK, RESET) 
        begin
            if (RESET = '1') then
                curr_state <= s_start;
            elsif (CLK'event) and (CLK = '1') then
                if (EN = '1') then 
                    curr_state <= next_state;
                end if;
            end if; 
    end process;
    --FSM state sync--


    --FSM next state logic--
    fsm_nsl : process(CLK, RESET) is begin
        OUT_INV <= '0';
        OUT_WE <= '0';
        IN_REQ <= '0';
        DATA_RDWR <= '1';
        DATA_EN <= '0';
        MUX1_SEL <= '0';
        MUX2_SEL <= "00";
        CNT_INC <= '0';
        CNT_DEC <= '0';
        PTR_INC <= '0';
        PTR_DEC <= '0';
        PTR_RST <= '0';
        PC_INC <= '0';
        PC_DEC <= '0';

        case curr_state is
            when s_start =>
                READY <= '0';
                DONE <= '0';
                OUT_DATA <= (others => '0');
                next_state <= s_init;
            when s_init =>
                if DATA_RDATA = X"40" then
                    READY <= '1';
                    next_state <= s_fetch_pc;
                else
                    DATA_EN <= '1';
                    DATA_RDWR <= '1';
                    MUX1_SEL <= '1';
                    PTR_INC <= '1';
                    next_state <= s_init;
                end if;
            when s_fetch_ptr =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                MUX1_SEL <= '1';
                next_state <= s_decode;
            when s_fetch_pc =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                MUX1_SEL <= '0';
                next_state <= s_decode;
            when s_decode =>
                case DATA_RDATA is
                    when X"40" =>
                        next_state <= s_halt;
                    when X"2B" =>   -- + increment value
                        MUX1_SEL <= '1';
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        next_state <= s_increment_value;
                    when X"2D" => -- - decrement value
                        MUX1_SEL <= '1';
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        next_state <= s_decrement_value;
                    when X"3E" => -- > increemnt pointer
                        PC_INC <= '1';
                        PTR_INC <= '1';
                        next_state <= s_fetch_pc;
                    when X"3C" => -- < decrement pointer
                        PC_INC <= '1';
                        PTR_DEC <= '1';
                        next_state <= s_fetch_pc;
                    when X"2E" => -- . print value
                        MUX1_SEL <= '1';
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        next_state <= s_print;
                    when X"2C" => -- , load value
                        IN_REQ <= '1';
                        if IN_VLD = '1' then
                            MUX2_SEL <= "11";
                            DATA_RDWR <= '0';
                            DATA_EN <= '1';
                            PC_INC <= '1';
                            MUX1_SEL <= '1';
                            next_state <= s_fetch_pc;
                        end if;
                    when X"5B" => -- [ start while loop
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        MUX1_SEL <= '1';
                        next_state <= s_while_check_start;
                    when X"5D" => -- ] end while loop
                        MUX1_SEL <= '1';
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        next_state <= s_while_end;
                    when X"29" => -- ) end do loop
                        MUX1_SEL <= '1';
                        DATA_EN <= '1';
                        DATA_RDWR <= '1';
                        next_state <= s_do_end;
                    when X"30" | X"31" | X"32" | X"33" | X"34" | X"35" | X"36" | X"37" | X"38" | X"39" | 
                        X"41" | X"42" | X"43" | X"44" | X"45" | X"46" => -- 0-9, A-F load hex
                        next_state <= s_write_hex;
                    when others =>
                        PC_INC <= '1';
                        next_state <= s_fetch_pc;
                end case;
            when s_halt =>
                DONE <= '1';
                next_state <= s_halt;
            when s_increment_value =>
                MUX1_SEL <= '1';
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                MUX2_SEL <= "01";
                PC_INC <= '1';
                next_state <= s_fetch_pc;
            when s_decrement_value =>
                MUX1_SEL <= '1';
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                MUX2_SEL <= "10";
                PC_INC <= '1';
                next_state <= s_fetch_pc;
            when s_print =>
                if OUT_BUSY = '0' then
                    OUT_DATA <= DATA_RDATA;
                    OUT_WE <= '1';
                    PC_INC <= '1';
                    next_state <= s_fetch_pc;
                end if;
            when s_write_hex =>
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                MUX1_SEL <= '1';
                MUX2_SEL <= "00";
                PC_INC <= '1';
                next_state <= s_fetch_pc;
            when s_while_check_start =>
                if DATA_RDATA = X"00" then --skip to thile end
                    PC_INC <= '1';
                    next_state <= s_while_skip_fetch;
                else        -- while
                    PC_INC <= '1';
                    next_state <= s_fetch_pc;
                end if;
            when s_while_skip_fetch =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                MUX1_SEL <= '0';
                next_state <= s_while_skip_check;
            when s_while_skip_check =>
                if DATA_RDATA = X"5B" then
                    CNT_INC <= '1';
                    PC_INC <= '1';
                    next_state <= s_while_skip_fetch;
                elsif DATA_RDATA = X"5D" then
                    if CNT = X"00" then
                        PC_INC <= '1';
                        next_state <= s_fetch_pc;
                    else
                        CNT_DEC <= '1';
                        PC_INC <= '1';
                        next_state <= s_while_skip_fetch;
                    end if;
                else
                    PC_INC <= '1';
                    next_state <= s_while_skip_fetch;
                end if;
            when s_while_end =>
                if DATA_RDATA = X"00" then
                    PC_INC <= '1';
                    next_state <= s_fetch_pc;
                else
                    PC_DEC <= '1';
                    next_state <= s_while_pc_fetch;
                end if;
            when s_while_pc_fetch =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                MUX1_SEL <= '0';
                next_state <= s_while_find_start;
            when s_while_find_start =>
                if DATA_RDATA = X"5B" then -- [
                    if CNT = X"00" then
                        PC_INC <= '1';
                        next_state <= s_fetch_pc;
                    else
                        CNT_DEC <= '1';
                        PC_DEC <= '1';
                        next_state <= s_while_pc_fetch;
                    end if;
                elsif DATA_RDATA = X"5D" then -- ]
                    PC_DEC <= '1';
                    CNT_INC <= '1';
                    next_state <= s_while_pc_fetch;
                else
                    PC_DEC <= '1';
                    next_state <= s_while_pc_fetch;
                end if;
            when s_do_end =>
                if DATA_RDATA = X"00" then
                    PC_INC <= '1';
                    next_state <= s_fetch_pc;
                else
                    PC_DEC <= '1';
                    next_state <= s_do_pc_fetch;
                end if;
            when s_do_pc_fetch =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                MUX1_SEL <= '0';
                next_state <= s_do_find_start;
            when s_do_find_start =>
                if DATA_RDATA = X"28" then -- (
                    if CNT = X"00" then
                        PC_INC <= '1';
                        next_state <= s_fetch_pc;
                    else
                        CNT_DEC <= '1';
                        PC_DEC <= '1';
                        next_state <= s_do_pc_fetch;
                    end if;
                elsif DATA_RDATA = X"29" then -- )
                    PC_DEC <= '1';
                    CNT_INC <= '1';
                    next_state <= s_do_pc_fetch;
                else
                    PC_DEC <= '1';
                    next_state <= s_do_pc_fetch;
                end if;
        end case;
    end process;

    --FSM--
    

 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.

end behavioral;