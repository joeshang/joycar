/**
 * File: gui_callbacks.c
 * Author: Joe Shang
 * Brief: The callback handlers of gui programs.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "connector.h"
#include "connector_serial.h"

extern Connector *con;

/***********************************************************
*                   Main Window Related                    *
***********************************************************/
void on_main_window_destroy(GtkWidget *widget, gpointer user_data)
{
    GtkBuilder *builder = (GtkBuilder *)user_data;
    g_object_unref(G_OBJECT(builder));

    connector_destroy(con);

    gtk_main_quit();
}

/***********************************************************
*           Communication Configuration Related            *
***********************************************************/
void on_connect_btn_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkComboBox *serial_port_combo;
    GtkComboBox *baud_rate_combo;

    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *serial_port = NULL;
    gchar *baud_rate = NULL;

    SerialArg con_arg;

    GtkBuilder *builder = (GtkBuilder *)user_data;

    serial_port_combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "serial_port_combo"));
    baud_rate_combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "baud_rate_combo"));

    if (gtk_combo_box_get_active_iter(serial_port_combo, &iter))
    {
        model = gtk_combo_box_get_model(serial_port_combo);

        gtk_tree_model_get(model, &iter, 0, &serial_port, -1);

        if (serial_port != NULL)
        {
            printf("serial port: %s\n", (char *)serial_port);
            con_arg.serial_port = serial_port;
        }
    }

    if (gtk_combo_box_get_active_iter(baud_rate_combo, &iter))
    {
        model = gtk_combo_box_get_model(baud_rate_combo);

        gtk_tree_model_get(model, &iter, 0, &baud_rate, -1);

        if (baud_rate != NULL)
        {
            printf("baud rate: %s\n", (char *)baud_rate);
            con_arg.baud_rate = atoi((char *)baud_rate);
        }
    }

    connector_open(con, &con_arg);
}

void on_disconnect_btn_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkBuilder *builder = (GtkBuilder *)user_data;
    GtkWidget *connect_btn = widget;
    GtkWidget *disconnect_btn;
    GtkImage *status_img;

    connector_close(con);

    disconnect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "disconnect_btn"));
    gtk_widget_set_sensitive(disconnect_btn, FALSE);

    connect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "connect_btn"));
    gtk_widget_set_sensitive(connect_btn, TRUE);

    status_img = GTK_IMAGE(gtk_builder_get_object(builder, "connect_status_img"));
    //gtk_image_set_from_file(status_img, "res/circle_green.png");
    gtk_image_set_from_icon_name(status_img, "dialog-error", GTK_ICON_SIZE_MENU);
}

/***********************************************************
*                 Motor Control Related                    *
***********************************************************/
void on_motor_forward_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mf]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_motor_backward_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mb]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_motor_turnleft_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[ml]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_motor_turnright_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mr]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_motor_btn_released(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mt]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_motor_speed_spin_value_changed(GtkWidget *widget, gpointer user_data)
{
    char msg_buf[10];
    GtkSpinButton *spinButton = (GtkSpinButton *)widget;
    gdouble speed = gtk_spin_button_get_value(spinButton);
    printf("motor speed %d\n", (int)speed);
    
    sprintf(msg_buf, "[ms%d]", (int)speed);
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

/***********************************************************
*                Holder Control Related                    *
***********************************************************/
void on_holder_turnup_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hu]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_holder_turndown_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hd]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_holder_turnleft_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hl]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_holder_turnright_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hr]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_holder_btn_released(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[ht]";
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

void on_holder_speed_spin_value_changed(GtkWidget *widget, gpointer user_data)
{
    char msg_buf[10];
    GtkSpinButton *spinButton = (GtkSpinButton *)widget;

    gdouble speed = gtk_spin_button_get_value(spinButton);
    printf("holder speed %d\n", (int)speed);

    sprintf(msg_buf, "[hs%d]", (int)speed);
    connector_send(con, msg_buf, strlen(msg_buf));

    printf("%s\n", msg_buf);
}

