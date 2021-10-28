#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include "dbHandler.h"
#include "typedefs.h"
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <assert.h>
#include "corpusTree.h"
#include "facialRecognitionAPI.h"
#include "NLP.h"

#define TITLE "Persons Affected by Natural Disasters Reunification"
#define LOGO "Pandr_logo.png"
#define MAX_RESULTS 30
// Make them global

GtkWidget *window;
GtkWidget *fixed1;
GtkWidget *firstNameLbl;
GtkWidget *lastNameLbl;
GtkWidget *imaegLbl;
GtkWidget *descrLbl;
GtkWidget *nameTxt;
GtkWidget *fileInput;
GtkWidget *descrTxt;
GtkWidget *findButton;
GtkWidget *resultLbl;
GtkWidget *popup;
GtkWidget *popup1;
GtkWidget *image;
GtkWidget *fixed2;
GtkFileFilter *imageOnly;
GtkBuilder	*builder;
char firstName[512];
char lastName[512];
char description[1024];




int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);


// establish contact with xml code used to adjust widget settings

 
	builder = gtk_builder_new_from_file ("findMissingPerson.glade");
 
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	findButton = GTK_WIDGET(gtk_builder_get_object(builder, "findButton"));
	firstNameLbl = GTK_WIDGET(gtk_builder_get_object(builder, "firstNameLbl"));
	lastNameLbl = GTK_WIDGET(gtk_builder_get_object(builder, "lastNameLbl"));
	descrLbl = GTK_WIDGET(gtk_builder_get_object(builder, "descrLbl"));
	imaegLbl = GTK_WIDGET(gtk_builder_get_object(builder, "imageLbl"));
	nameTxt = GTK_WIDGET(gtk_builder_get_object(builder, "nameTxt"));
	descrTxt = GTK_WIDGET(gtk_builder_get_object(builder, "descrTxt"));
	fileInput = GTK_WIDGET(gtk_builder_get_object(builder, "fileInput"));
	resultLbl = GTK_WIDGET(gtk_builder_get_object(builder, "resultLbl"));
	popup = GTK_WIDGET(gtk_builder_get_object(builder, "popup"));
	image = GTK_WIDGET(gtk_builder_get_object(builder, "resultimg"));
	fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
	popup1 = GTK_WIDGET(gtk_builder_get_object(builder, "popup1"));
	imageOnly =  gtk_file_filter_new();
	gtk_file_filter_set_name(imageOnly, "IMAGE");
	gtk_file_filter_add_pixbuf_formats(imageOnly);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fileInput), imageOnly);
    gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(window))), (const gchar*) TITLE);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(window))), TRUE);
	gtk_window_set_icon(GTK_WINDOW(window), gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file(LOGO))));
	gtk_window_set_default_size(GTK_WINDOW(window), 650, 500);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(popup))), (const gchar*) TITLE);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(popup))), TRUE);
	gtk_window_set_icon(GTK_WINDOW(popup), gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file(LOGO))));
	gtk_window_set_default_size(GTK_WINDOW(popup), 650, 500);
	gtk_header_bar_set_title(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(popup1))), (const gchar*) TITLE);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(gtk_window_get_titlebar(GTK_WINDOW(popup1))), TRUE);
	gtk_window_set_icon(GTK_WINDOW(popup1), gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file(LOGO))));



	gtk_widget_show(window);

	gtk_main();

	return EXIT_SUCCESS;
}

void on_firstNameTxt_changed (GtkEntry *e) {
	sprintf(firstName, "%s",  gtk_entry_get_text(e));
}

void on_lastNameTxt_changed (GtkEntry *e) {
	sprintf(lastName, "%s",  gtk_entry_get_text(e));
}


void on_descrTxt_changed (GtkEntry *e) {
	sprintf(description, "%s",  gtk_entry_get_text(e));
}


void on_findButton_clicked (GtkButton *b, gpointer user_data) {
	char * fileURL = (char *) gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileInput));
	if (fileURL == NULL) {
		puts("Image not found");
		return;
	}
	if (!openDatabase()) {
		puts("error opening database");
		return;
	}
	person *results = calloc(MAX_RESULTS, sizeof(person));

	if (!get_from_db(results, firstName, lastName)) {
		puts("error retrieving from database");
	}

	person *topScorer = malloc(sizeof(person));
	int topConfidence = 0;
	int confidence = 0;
	int faceMatchPercentage = 0;
	double descMatchPercantage = 0;
	for (int i = 0; i < MAX_RESULTS ; i++) {
		if (results[i].firstname == NULL) {
			break;
		}
		faceMatchPercentage = percentageMatch(fileURL, results[i].imagePath);
		// printf("percentage: %d\n", faceMatchPercentage);
		descMatchPercantage = runNlpProcess(description, results[i].description);
		// printf("nlp: %f\n", descMatchPercantage);
		confidence = ((0.65 * faceMatchPercentage) + (0.35 * descMatchPercantage)) * 100;
		if (confidence > topConfidence){
			topScorer = &results[i];
			topConfidence = confidence;
		}
	}

	char label[128];
	if (topConfidence < 50) {
		strcpy(label, "Unfortunately no person has been found to match the person you specified.");
	} else {
		sprintf(label, "This is the person who is most like the one you inputted. The percentage match is: %d. They are located at %s", topConfidence, topScorer->location);
		GtkWidget *resultImg = gtk_image_new_from_file((const gchar *) topScorer->imagePath);
		gtk_container_add(GTK_CONTAINER (fixed2), resultImg);
		gtk_label_set_text(GTK_LABEL(resultLbl), label);
		gtk_widget_show(popup);
		gtk_widget_show(resultImg);
	}
	gtk_label_set_text(GTK_LABEL(resultLbl), label);
	gtk_widget_show(popup1);
}