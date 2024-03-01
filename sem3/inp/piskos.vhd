-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2023 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Lukáš Píšek <xpisek02@stud.fit.vutbr.cz>
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
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
  -- CNT
  signal CNT : std_logic_vector(7 downto 0); 
  signal CNT_INC  : std_logic;
  signal CNT_DEC  : std_logic;
  signal CNT_LOAD : std_logic;

  -- PTR
  signal PTR      : std_logic_vector(12 downto 0);
  signal PTR_INC  : std_logic;
  signal PTR_DEC  : std_logic;

  -- PC
  signal PC       : std_logic_vector(12 downto 0);
  signal PC_INC   : std_logic;
  signal PC_DEC   : std_logic;

  -- MX1
  signal MX1_SEL  : std_logic;

  -- MX2
  signal MX2_SEL  : std_logic_vector(1 downto 0);

  type State_T is (idle, pause, loadInst, init, instrRead, incrementRead, incrementWrite, decrementRead, decrementWrite, printRead, printWrite, readWait, readWrite, loopStart, stall, stall1, looping, loopRight, stall2, loopingRight, stall3, stallBreak, break, halt);
  signal State : State_T := idle;

begin

  P_CNT : process(CLK, RESET)
  begin
    if RESET = '1' then
      CNT <= (others => '0');
    elsif rising_edge(CLK) then
      if (CNT_INC = '1') then
        CNT <= CNT + 1;
      elsif (CNT_DEC = '1') then
        CNT <= CNT - 1;
      end if;
    end if;
  end process P_CNT;

  P_PTR : process(CLK, RESET)
  begin
    if RESET = '1' then
      PTR <= (others => '0');
    elsif rising_edge(CLK) then
      if (PTR_INC = '1') then
        PTR <= PTR + 1;
      elsif (PTR_DEC = '1') then
        PTR <= PTR - 1;
      end if;
    end if;
  end process P_PTR;

  P_PC : process(CLK, RESET)
  begin
    if RESET = '1' then
      PC <= (others => '0');
    elsif rising_edge(CLK) then
      if (PC_INC = '1') then
        PC <= PC + 1;
      elsif (PC_DEC = '1') then
        PC <= PC - 1;
      end if;
    end if;
  end process P_PC;

  MX1 : process (CLK, MX1_SEL)
  begin
    if (MX1_SEL = '0') then
      DATA_ADDR <= PTR; -- data
    else
      DATA_ADDR <= PC; -- instrukce
    end if;
  end process MX1;

  MX2: process (CLK, MX2_SEL)
  begin
    case (MX2_SEL) is
      when "00" => 
        DATA_WDATA <= IN_DATA;
      when "01" => 
        DATA_WDATA <= DATA_RDATA;
      when "10" =>  
        DATA_WDATA <= DATA_RDATA - 1;
      when "11" => 
        DATA_WDATA <= DATA_RDATA + 1;
      when others => null;
    end case;
  end process MX2;

  FSM : process(CLK, RESET)
  begin
    if (RESET = '1') then
      State <= idle;
      READY <= '0';
      DONE <= '0';
      DATA_EN <= '0';
      IN_REQ <= '0';
      OUT_WE <= '0';
      MX1_SEL <= '0';
      PTR_INC <= '0';
      PTR_DEC <= '0';
      PC_INC <= '0';
      PC_DEC <= '0';
      CNT_INC <= '0';
      CNT_DEC <= '0';
      CNT_LOAD <= '0';
    elsif rising_edge(CLK) then
      case State is

        when idle =>
          if (EN = '1') then
            State <= init;
          end if;

        when init =>
            DATA_EN <= '1';
            DATA_RDWR <= '0';
            MX1_SEL <= '0';
            if (DATA_RDATA = X"40") then
              PTR_INC <= '0';
              PTR_DEC <= '1';
              PC_INC <= '0';
              State <= pause;
              MX1_SEL <= '1';
              READY <= '1';

            else
              PTR_INC <= '1';
            end if;

        when pause =>
          State <= loadInst;
          MX1_SEL <= '1';
          OUT_WE <= '0';
          PTR_INC <= '0';
          PTR_DEC <= '0';
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          PC_INC <= '0';
          IN_REQ <= '0';
          PTR_DEC <= '0';
            
        when loadInst =>
              State <= instrRead;


        when instrRead =>
          case (DATA_RDATA) is
            when X"3E" => -- >
              PTR_INC <= '1';
              PC_INC <= '1';
              State <= pause;
              
            when X"3C" => -- <
              PTR_DEC <= '1';
              PC_INC <= '1';
              State <= pause;

            when X"2B" => -- +
              State <= incrementRead;
              MX1_SEL <= '0';

            when X"2D" => -- -
              State <= decrementRead;
              MX1_SEL <= '0';

            when X"2E" => -- .
              State <= printRead;

              MX1_SEL <= '0';

            when X"2C" => -- ,
              State <= readWait;
              IN_REQ <= '1';
              
            when X"5B" => -- [
              State <= stall;
              MX1_SEL <= '0';

            when X"5D" => -- ]
              State <= stall2;
              MX1_SEL <= '0';

            when X"40" => -- @
              State <= halt;
              

            when X"7E" => -- ~
              PC_INC <= '1';
              State <= stallBreak;

            when others =>
              PC_INC <= '1';
              State <= pause;
          end case ;

        when incrementRead =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          State <= incrementWrite;

        when incrementWrite =>
          DATA_RDWR <= '1';
          MX2_SEL <= "11";
          PC_INC <= '1';
          State <= pause;

        when decrementRead =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          State <= decrementWrite;

        when decrementWrite =>
          DATA_RDWR <= '1';
          MX2_SEL <= "10";
          PC_INC <= '1';
          State <= pause;
          
        when printRead =>
          DATA_RDWR <= '0';
          DATA_EN <= '1';
          State <= printWrite;

        when printWrite =>
          if (OUT_BUSY = '1') then
            State <= printWrite;
          else
            OUT_WE <= '1';
            OUT_DATA <= DATA_RDATA;
            MX1_SEL <= '1';
            PC_INC <= '1';
            State <= pause;
          end if;

        when readWait =>
          MX1_SEL <= '0';
          if (IN_VLD = '1') then
            MX2_SEL <= "00";
            MX1_SEL <= '0';
            DATA_RDWR <= '1';
            State <= readWrite;
          else
            State <= readWait;
          end if;

        when readWrite =>
          PC_INC <= '1';
          MX1_SEL <= '1';
          DATA_RDWR <= '0';
          MX2_SEL <= "01";
          State <= pause;
          
        when stall =>
          State <= loopStart;

        when loopStart =>
          if (DATA_RDATA = X"00") then
            PC_INC <= '1';
            MX1_SEL <= '1';
            State <= stall1;
          else
            PC_INC <= '1';
            MX1_SEL <= '1';
            State <= pause;
          end if;
        
        when stall1 =>
          PC_INC <= '0';
          State <= looping;

        when looping =>
          PC_INC <= '1';
          if (DATA_RDATA = X"5D") then
            PC_INC <= '0';
            State <= pause;
          else
            State <= stall1;
          end if;
        
        when stall2 =>
          State <= loopRight;

        when loopRight =>
          if (DATA_RDATA = X"00") then
            State <= pause;
            PC_INC <= '1';
          else
            PC_DEC <= '1';
            MX1_SEL <= '1';
            State <= stall3;
          end if;
            
        when stall3 =>
          PC_DEC <= '0';
          State <= loopingRight;

        when loopingRight =>
          if (DATA_RDATA = X"5B") then
            PC_INC <= '1';
            State <= pause;
            MX1_SEL <= '1';
          else
            PC_DEC <= '1';
            State <= stall3;
          end if;
          
        when stallBreak =>
          PC_INC <= '0';
          State <= break;
          
        when break =>
            if (DATA_RDATA = X"5D") then
              State <= pause;
            else
              PC_INC <= '1';
              State <= stallBreak;
            end if;

        when halt =>
          DONE <= '1';

        when others => 
          State <= idle;

      end case;
     
    end if ;
  
  end process FSM;


 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly. 

end behavioral;
