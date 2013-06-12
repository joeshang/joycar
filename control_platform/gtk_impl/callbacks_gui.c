/**
 * File: callbacks_gui.c
 * Author: Joe Shang
 * Brief: The callback handlers of gui programs.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"
#include "connector.h"
#include "connector_net.h"
#include "connector_serial.h"

#include "callbacks_con.h"

enum CON_TYPE
{
    CON_SERIAL,
    CON_NET
};

extern Decoder *frame_decoder;
extern Connector *active_connector; 

static int saved_con_type = CON_SERIAL;

/***********************************************************
*                   Main Window Related                    *
***********************************************************/
void on_main_window_destroy(GtkWidget *widget, gpointer user_data)
{
    GtkBuilder *builder = (GtkBuilder *)user_data;
    g_object_unref(G_OBJECT(builder));

    if (active_connector != NULL)
    {
        connector_destroy(active_connector);
    }

    if (frame_decoder != NULL)
    {
        decoder_destroy(frame_decoder);
    }

    gtk_main_quit();
}

/***********************************************************
*           Communication Configuration Related            *
***********************************************************/
static void on_serial_connection_open(GtkBuilder *builder)
{
    GtkComboBox *serial_port_combo;
    GtkComboBox *baud_rate_combo;

    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *serial_port = NULL;
    gchar *baud_rate = NULL;

    SerialArg con_arg;

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

    if (serial_port != NULL && baud_rate != NULL)
    {
        connector_open(active_connector, 
                &con_arg, 
                connector_open_callback, 
                (void *)builder);
    }
    else
    {
        /* TODO: handle invalid connection argument */
    }
}

static void on_net_connection_open(GtkBuilder *builder)
{
    GtkEntry *addr_entry;
    GtkEntry *port_entry;

    char *addr;
    char *port;

    NetworkArg con_arg;

    addr_entry = GTK_ENTRY(gtk_builder_get_object(builder, "net_addr_entry"));
    if ((addr = (char *)gtk_entry_get_text(addr_entry)) != NULL)
    {
        printf("net address: %s\n", addr);
        con_arg.net_addr = addr;
    }

    port_entry = GTK_ENTRY(gtk_builder_get_object(builder, "net_port_entry"));
    if ((port = (char *)gtk_entry_get_text(port_entry)) != NULL)
    {
        printf("net portess: %s\n", port);
        con_arg.net_port = atoi(port);
    }
    
    if (addr != NULL && port != NULL)
    {
        connector_open(active_connector,
                &con_arg,
                connector_open_callback,
                (void *)builder);
    }
    else
    {
        /* TODO: handle invalid connection argument */
    }
}

void on_connect_btn_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkNotebook *con_notebook;
    int sel_con_type;

    GtkBuilder *builder = (GtkBuilder *)user_data;

    con_notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "config_notebook"));
    sel_con_type = gtk_notebook_get_current_page(con_notebook);

    if (active_connector != NULL && saved_con_type != sel_con_type)
    {
        connector_destroy(active_connector);
        active_connector = NULL;
    }
    saved_con_type = sel_con_type;

    switch (sel_con_type)
    {
        case CON_SERIAL:
            printf("\nConnection with serial\n");
            if (active_connector == NULL)
            {
                active_connector = connector_serial_create();
            }
            on_serial_connection_open(builder);
            break;
        case CON_NET:
            printf("\nConnection with network\n");
            if (active_connector == NULL)
            {
                active_connector = connector_net_create();
            }
            on_net_connection_open(builder);
            break;
        default:
            break;
    }
}

void on_disconnect_btn_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkBuilder *builder = (GtkBuilder *)user_data;
    GtkWidget *connect_btn = widget;
    GtkWidget *disconnect_btn;
    GtkImage *status_img;

    connector_close(active_connector, connector_close_callback, user_data);

    disconnect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "disconnect_btn"));
    gtk_widget_set_sensitive(disconnect_btn, FALSE);

    connect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "connect_btn"));
    gtk_widget_set_sensitive(connect_btn, TRUE);

    status_img = GTK_IMAGE(gtk_builder_get_object(builder, "connect_status_img"));
    gtk_image_set_from_icon_name(status_img, "dialog-error", GTK_ICON_SIZE_MENU);
}

/***********************************************************
*                 Motor Control Related                    *
***********************************************************/
void on_motor_forward_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mf]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_motor_backward_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mb]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_motor_turnleft_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[ml]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_motor_turnright_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mr]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_motor_btn_released(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[mt]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_motor_speed_spin_value_changed(GtkWidget *widget, gpointer user_data)
{
    char msg_buf[10];
    GtkSpinButton *spinButton = (GtkSpinButton *)widget;
    gdouble speed = gtk_spin_button_get_value(spinButton);
    printf("motor speed %d\n", (int)speed);
    
    sprintf(msg_buf, "[ms%d]", (int)speed);
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

/***********************************************************
*                Holder Control Related                    *
***********************************************************/
void on_holder_turnup_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hu]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_holder_turndown_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hd]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_holder_turnleft_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hl]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_holder_turnright_btn_pressed(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[hr]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_holder_btn_released(GtkWidget *widget, gpointer user_data)
{
    char *msg_buf = "[ht]";
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

void on_holder_speed_spin_value_changed(GtkWidget *widget, gpointer user_data)
{
    char msg_buf[10];
    GtkSpinButton *spinButton = (GtkSpinButton *)widget;

    gdouble speed = gtk_spin_button_get_value(spinButton);
    printf("holder speed %d\n", (int)speed);

    sprintf(msg_buf, "[hs%d]", (int)speed);
    connector_send(active_connector, msg_buf, strlen(msg_buf));
}

