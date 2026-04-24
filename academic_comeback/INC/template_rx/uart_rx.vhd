-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Name Surname (xlogin00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;



-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is
signal cnt1 : std_logic_vector(3 downto 0) := (others => '0');
signal cnt2 : std_logic_vector(3 downto 0) := (others => '0');
signal fsm_state : std_logic_vector(1 downto 0);
signal data : std_logic_vector(7 downto 0) := (others => '0');
begin

    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK => CLK,
        RST => RST,
        DIN => DIN,
        CNT1 => cnt1,
        CNT2 => cnt2,
        STATE => fsm_state
    );

    counters: process(CLK, RST)
    begin
        if RST = '1' then
            cnt1 <= (others => '0');
            cnt2 <= (others => '0');
            data <= (others => '0');
            DOUT_VLD <= '0';
        elsif rising_edge(CLK) then
            DOUT_VLD <= '0';

            if fsm_state = "01" or fsm_state = "10" then
                cnt1 <= cnt1 + 1;
            else
                cnt1 <= (others => '0');
            end if;
            
            if  cnt1 = "1000" then
                if fsm_state = "01" then
                    data(0) <= DIN;
                    cnt2 <= "0000";
                elsif fsm_state = "10" then
                    data <= DIN & data(7 downto 1);
                    cnt2 <= cnt2 + 1;
                end if;
            end if;

            if fsm_state = "10" and cnt2 = "1000" and cnt1 = "1111" then
                DOUT_VLD <= '1';
            end if;
        end if;
    end process;

    DOUT <= data;

end architecture;
