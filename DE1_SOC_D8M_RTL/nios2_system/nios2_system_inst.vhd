	component nios2_system is
		port (
			clk_clk         : in  std_logic                     := 'X';             -- clk
			coords_ram_addr : out std_logic_vector(4 downto 0);                     -- addr
			coords_ram_data : in  std_logic_vector(31 downto 0) := (others => 'X'); -- data
			esp_uart_rxd    : in  std_logic                     := 'X';             -- rxd
			esp_uart_txd    : out std_logic;                                        -- txd
			reset_reset_n   : in  std_logic                     := 'X'              -- reset_n
		);
	end component nios2_system;

	u0 : component nios2_system
		port map (
			clk_clk         => CONNECTED_TO_clk_clk,         --        clk.clk
			coords_ram_addr => CONNECTED_TO_coords_ram_addr, -- coords_ram.addr
			coords_ram_data => CONNECTED_TO_coords_ram_data, --           .data
			esp_uart_rxd    => CONNECTED_TO_esp_uart_rxd,    --   esp_uart.rxd
			esp_uart_txd    => CONNECTED_TO_esp_uart_txd,    --           .txd
			reset_reset_n   => CONNECTED_TO_reset_reset_n    --      reset.reset_n
		);

