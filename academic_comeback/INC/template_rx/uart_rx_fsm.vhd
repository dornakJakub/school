-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Jakub Dornak (xdorna06)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



entity UART_RX_FSM is
    port(
       CLK : in std_logic;
       RST : in std_logic;
       DIN : in std_logic;
       CNT1 : in std_logic_vector(3 downto 0);
       CNT2 : in std_logic_vector(3 downto 0);
       STATE : out std_logic_vector(1 downto 0)
   );
end entity;



architecture behavioral of UART_RX_FSM is
    type STATE_TYPE is (WAIT_START, START_RECEIVE, RECEIVE, READ_STOP);
    signal curr_state : STATE_TYPE := WAIT_START;
begin

    process (CLK) begin
        if rising_edge(CLK) then
            if RST = '1' then
                curr_state <= WAIT_START;
            else
                case curr_state is
                    when WAIT_START => if DIN = '0' then
                        curr_state <= START_RECEIVE;
                    end if;
                    when START_RECEIVE => if CNT1 = "1111" then
                        curr_state <= RECEIVE;
                    end if;
                    when RECEIVE => if CNT2 = "1000" and CNT1 = "1111" then
                        curr_state <= READ_STOP;
                    end if;
                    when READ_STOP => if DIN = '1' then
                            curr_state <= WAIT_START;
                    end if;
                end case;
            end if;
        end if;
    end process;

    STATE <= "00" when curr_state = WAIT_START else
            "01" when curr_state = START_RECEIVE else
            "10" when curr_state = RECEIVE else
            "11" when curr_state = READ_STOP else
            "00";

end architecture;
