#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include <libgen.h>
#include "dbHandler.h"
#include "typedefs.h"

#define TITLE "Persons Affected by Natural Disasters Reunification"
#define LOGO "Pandr_logo.png"
#define LOCAL_IMAGE_PATH "images/"
// Make them global

GtkWidget *window;
GtkWidget *fixed1;
GtkWidget *firstNameLbl;
GtkWidget *lastNameLbl;
GtkWidget *locationLbl;
GtkWidget *imaegLbl;
GtkWidget *descrLbl;
GtkWidget *firstNameTxt;
GtkWidget *lastNameTxt;
GtkWidget *locationTxt;
GtkWidget *fileInput;
GtkWidget *descrTxt;
GtkWidget *addButton;
GtkFileFilter *imageOnly;
GtkBuilder	*builder;
char firstName[512];
char lastName[512];
char location[512];
char description[1024];


int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);


// establish contact with xml code used to adjust widget settings

 
	builder = gtk_builder_new_from_file ("addMissingPerson.glade");
 
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	addButton = GTK_WIDGET(gtk_builder_get_object(builder, "addButton"));
	firstNameLbl = GTK_WIDGET(gtk_builder_get_object(builder, "firstNameLbl"));
	lastNameLbl = GTK_WIDGET(gtk_builder_get_object(builder, "lastNameLbl"));
	locationLbl = GTK_WIDGET(gtk_builder_get_object(builder, "locationLbl"));
	descrLbl = GTK_WIDGET(gtk_builder_get_object(builder, "descrLbl"));
	imaegLbl = GTK_WIDGET(gtk_builder_get_object(builder, "imageLbl"));
	firstNameTxt = GTK_WIDGET(gtk_builder_get_object(builder, "firstNameTxt"));
    lastNameTxt = GTK_WIDGET(gtk_builder_get_object(builder, "lastNameTxt"));
	locationTxt = GTK_WIDGET(gtk_builder_get_object(builder, "locationTxt"));
	descrTxt = GTK_WIDGET(gtk_builder_get_object(builder, "descrTxt"));
	fileInput = GTK_WIDGET(gtk_builder_get_object(builder, "fileInput"));
	imageOnly =  gtk_file_filter_new();
	gtk_file_filter_set_name(imageOnly, "IMAGE");
	gtk_file_filter_add_pixbuf_formats(imageOnly);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fileInput), imageOnly);
	gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(window))), (const gchar*) TITLE);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(window))), TRUE);
	gtk_window_set_icon(GTK_WINDOW(window), gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file(LOGO))));
	gtk_window_set_default_size(GTK_WINDOW(window), 675, 500);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	gtk_widget_show(window);

	gtk_main();

	return EXIT_SUCCESS;
}

int copyfile(char* input, char* output) {
	FILE* fp1 = fopen(input, "r");
    FILE* fp2 = fopen(output, "w");
	if (!fp1 || !fp2) {
		puts("File not found or could not be created.");
		return 0;
	}
    int c = 0;
	while((c = getw(fp1)) != EOF) {
		fwrite(&c, sizeof(int), 1, fp2); 
	}
	fclose(fp1);
	fclose(fp2);
	return 1;
}
void on_firstNameTxt_changed (GtkEntry *e) {
	sprintf(firstName, "%s",  gtk_entry_get_text(e));
}

void on_lastNameTxt_changed (GtkEntry *e) {
	sprintf(lastName, "%s",  gtk_entry_get_text(e));
}


void on_locationTxt_changed (GtkEntry *e) {
	sprintf(location, "%s",  gtk_entry_get_text(e));
}


void on_descrTxt_changed (GtkEntry *e) {
	sprintf(description, "%s",  gtk_entry_get_text(e));
}


void on_addButton_clicked (GtkButton *b, gpointer user_data) {
	char * fileURL = (char *) gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileInput));
	if (fileURL == NULL) {
		return;
	}
	char output[512];
	strcpy(output, LOCAL_IMAGE_PATH);
	strcat(output, basename(fileURL));
	if (access(output, F_OK) == 0)   {
		puts("An image with this name already exists");
		return;
	}
	if (!copyfile(fileURL, output)) {
		puts("Image unable to be copied");
		return;
	}

	if (!openDatabase()) {
		puts("Could not open database");
		return;
	}

	puts("");

	printf("%s\n", firstName);
	printf("%s\n", lastName);
	printf("%s\n", location);
	printf("%s\n", description);
	printf("%s\n", output);


	if (!add_to_db(firstName, lastName, location, description, output)) {
		puts("Could not add to database");
		return;
	}
	puts("Added to database");
	closeDatabase();
}