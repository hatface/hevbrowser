/* main.c
 * Heiher <admin@heiher.info>
 */

#include "hevmainwindow.h"

int main(int argc, char * argv[])
{
	GObject * main_window = NULL;

	gtk_init(&argc, &argv);

	main_window = hev_main_window_new();
	g_return_val_if_fail(main_window, 1);

	gtk_main();

	return 0;
}

