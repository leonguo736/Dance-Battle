	nios2_system u0 (
		.clk_clk         (<connected-to-clk_clk>),         //        clk.clk
		.coords_ram_addr (<connected-to-coords_ram_addr>), // coords_ram.addr
		.coords_ram_data (<connected-to-coords_ram_data>), //           .data
		.esp_uart_rxd    (<connected-to-esp_uart_rxd>),    //   esp_uart.rxd
		.esp_uart_txd    (<connected-to-esp_uart_txd>),    //           .txd
		.reset_reset_n   (<connected-to-reset_reset_n>)    //      reset.reset_n
	);

