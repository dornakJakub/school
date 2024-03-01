-- Copyright (C) 2023 Brno University of Technology,
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
 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly. 

  --------------------------------------------------------------
  --                  Definice signalu a stavu
  --------------------------------------------------------------
  type State_T is (IDLE, INIT, ACTIVE, DELAY, PREPARE, LOAD_INSTRUCTION, MEMORY_INCREMENT, MEMORY_INCREMENT_WRITE,MEMORY_DECREMENT, MEMORY_DECREMENT_WRITE, BREAK, MEMORY_PRINT, MEMORY_WRITE, HALT);  --Vycet stavu procesoru
  signal currState: State_T := IDLE;  --na zacatku zacni v necinnem stavu

    -- PTR
    signal PTR           : std_logic_vector(12 downto 0);
    signal PTR_INCREMENT : std_logic;
    signal PTR_DECREMENT : std_logic;

    -- PC
    signal PC            :std_logic_vector(12 downto 0);
    signal PC_INCREMENT  :std_logic;
    signal PC_DECREMENT  :std_logic;

    -- MX1
    signal MX1_SELECT    :std_logic;

    -- MX2
    signal MX2_SELECT    :std_logic_vector(1 downto 0);

begin
  --------------------------------------------------------------
  --               Jednotlive komponenty procesoru
  --------------------------------------------------------------

  P_PTR : process(CLK, RESET)
  begin
    if RESET = '1' then
      PTR <= (others => '0');
    elsif rising_edge(CLK) then
      if (PTR_INCREMENT = '1') then
        PTR <= PTR + 1;
      elsif (PTR_DECREMENT = '1') then
        PTR <= PTR - 1;
      end if;
    end if;
  end process P_PTR;
  
  C_PTR: process(CLK, RESET)
  begin
    if rising_edge(CLK) then
      if (PTR_INCREMENT = '1') then
        PTR <= PTR + '1';
      elsif (PTR_DECREMENT = '1') then
        PTR <= PTR - '1';
      end if;
    elsif RESET = '1' then
      PTR <= (others => '0');
    end if;
  end process C_PTR;

  C_PC: process(CLK, RESET)
  begin
    if rising_edge(CLK) then
      if (PC_INCREMENT = '1') then
        PC <= PC + '1';
      elsif (PC_DECREMENT = '1') then
        PC <= PC - '1';
      end if;
    elsif RESET = '1' then
      PC <= (others => '0');
    end if;
  end process C_PC;

  C_MX1: process(CLK, MX1_SELECT)
  begin
    if rising_edge(CLK) then
      if (MX1_SELECT = '0') then
        DATA_ADDR <= PTR;
      else
        DATA_ADDR <= PC;
      end if;
    end if;
  end process C_MX1;

  C_MX2: process(CLK, MX2_SELECT)
  begin
    if rising_edge(CLK) then
      if (MX2_SELECT = "00") then
        DATA_WDATA <= IN_DATA;
      elsif (MX2_SELECT = "01") then
        DATA_WDATA <= DATA_RDATA;
      elsif (MX2_SELECT = "10") then
        DATA_WDATA <= DATA_RDATA -1;
      elsif (MX2_SELECT = "11") then
        DATA_WDATA <= DATA_RDATA + 1;   
      end if;
    end if;
  end process C_MX2;
  --------------------------------------------------------------
  --                FSM pro prepinani stavu procesoru
  --------------------------------------------------------------
  FSM: process(CLK, RESET)
  begin
    if RESET = '1' then --Reset procesoru
      currState <= IDLE;
      PTR_INCREMENT <= '0';
      PTR_DECREMENT <= '0';
      PC_INCREMENT <= '0';
      PC_DECREMENT <= '0';
      READY <= '0';
      DONE <= '0';
      DATA_EN <= '0';
      IN_REQ <= '0';
      OUT_WE <= '0';
      MX1_SELECT <= '0';
    elsif rising_edge(CLK) then --Pokud neni reset, procesor reaguje na vzestupnou hranu hodin
      case currState is
        when IDLE =>
          if (EN = '1') then
            currState <= INIT;
          end if;
        when INIT =>
          DATA_RDWR <= '0';
          DATA_EN <= '1';
          MX1_SELECT <= '0';
          if DATA_RDATA = X"40" then  --Narazili jsme na @
            READY <= '1';
            MX1_SELECT <= '1';
            PTR_INCREMENT <= '0';
            currState <= PREPARE;
            PTR_DECREMENT <= '1';
            PC_INCREMENT <= '0';
          else
            PTR_INCREMENT <= '1';
          end if;
        when PREPARE =>
          currState <= DELAY;
          MX1_SELECT <= '1';
          OUT_WE <= '0';
          DATA_EN <= '1';
          PTR_INCREMENT <= '0';
          PTR_DECREMENT <= '0';
          PC_INCREMENT <= '0';
          PC_DECREMENT <= '0';
          DATA_RDWR <= '0';
          IN_REQ <= '0';
        when DElAY =>   --zpozdeni aby se stihli precist/zapsat data
          currState <= LOAD_INSTRUCTION;
        when LOAD_INSTRUCTION =>
          case (DATA_RDATA) is  --Cteni jednotlivych instrukci
            when X"3E" =>  -- >
              PTR_INCREMENT <= '1';
              PC_INCREMENT <= '1';
              currState <= PREPARE;
            when X"3C" => -- <
              PTR_DECREMENT <= '1';
              PC_INCREMENT <= '1';
              currstate <= PREPARE;
            when X"2B" => -- +
              currState <= MEMORY_INCREMENT;
              MX1_SELECT <= '0';
            when X"2D" => -- -
              currState <= MEMORY_DECREMENT;
              MX1_SELECT <= '0';
            when X"5B" => -- [
              
            when X"5D" => -- ]
              
            when X"7E" => -- ~
              currState <= BREAK;
            when X"2E" => -- .
              currState <= MEMORY_PRINT;
            when X"2C" => -- ,
              currState <= MEMORY_WRITE;
            when X"40" => -- @
              currState <= HALT;
            when others =>
              PC_INCREMENT <= '1';
              currState <= PREPARE;
          end case;
        when MEMORY_INCREMENT =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          currState <= MEMORY_INCREMENT_WRITE;
        when MEMORY_INCREMENT_WRITE =>
          DATA_RDWR <= '1';
          PC_INCREMENT <= '1';
          MX2_SELECT <= "11";
          currState <= PREPARE;
        when MEMORY_DECREMENT =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          currState <= MEMORY_DECREMENT_WRITE;
        when MEMORY_DECREMENT_WRITE =>
          DATA_RDWR <= '1';
          PC_INCREMENT <= '1';
          MX2_SELECT <= "10";
          currState <= PREPARE;
        when HALT =>
          DONE <= '1';
        when others =>
          currState <= IDLE;
      end case;
    end if;
  end process FSM; 
end behavioral;
