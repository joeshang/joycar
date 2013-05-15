#include <gtk/gtk.h>

#define MAIN_WINDOW_BORDER              10
#define MAIN_VBOX_SPACING               5

#define CTRL_BTN_ROW_SPACING            4 
#define CTRL_BTN_COL_SPACING            4

enum
{
    ID_MOTOR_FORWARD,
    ID_MOTOR_BACKWARD,
    ID_MOTOR_TURNLEFT,
    ID_MOTOR_TURNRIGHT,
    ID_HOLDER_TURNUP,
    ID_HOLDER_TURNDOWN,
    ID_HOLDER_TURNLEFT,
    ID_HOLDER_TURNRIGHT
};

void btn_pressed_handler(GtkWidget *widget, gpointer data)
{
    int id = (int)data;

    switch (id)
    {
        case ID_MOTOR_FORWARD:
            break;
        case ID_MOTOR_BACKWARD:
            break;
        case ID_MOTOR_TURNLEFT:
            break;
        case ID_MOTOR_TURNRIGHT:
            break;
        case ID_HOLDER_TURNUP:
            break;
        case ID_HOLDER_TURNDOWN:
            break;
        case ID_HOLDER_TURNLEFT:
            break;
        case ID_HOLDER_TURNRIGHT:
            break;
        default:
            break;
    }
}

void btn_released_handler(GtkWidget *widget, gpointer data)
{
    int id = (int)data;

    switch (id)
    {
        case ID_MOTOR_FORWARD:
        case ID_MOTOR_BACKWARD:
        case ID_MOTOR_TURNLEFT:
        case ID_MOTOR_TURNRIGHT:
            printf("motor stop\n");
            break;
        case ID_HOLDER_TURNUP:
        case ID_HOLDER_TURNDOWN:
        case ID_HOLDER_TURNLEFT:
        case ID_HOLDER_TURNRIGHT:
            break;
        default:
            break;
    }
}


int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *main_vbox;

    GtkWidget *motor_frame;
    GtkWidget *motor_btn_table;
    GtkWidget *motor_forward_btn;
    GtkWidget *motor_backward_btn;
    GtkWidget *motor_turnleft_btn;
    GtkWidget *motor_turnright_btn;
    GtkWidget *motor_speed_label;
    GtkWidget *motor_speed_spin;
    GtkWidget *motor_speed_spin_adj;

    GtkWidget *holder_frame;
    GtkWidget *holder_btn_table;
    GtkWidget *holder_turnup_btn;
    GtkWidget *holder_turndown_btn;
    GtkWidget *holder_turnleft_btn;
    GtkWidget *holder_turnright_btn;
    GtkWidget *holder_speed_label;
    GtkWidget *holder_speed_spin;
    GtkWidget *holder_speed_spin_adj;

    gtk_init(&argc, &argv);

    /* main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), MAIN_VBOX_SPACING);
    gtk_window_set_title(GTK_WINDOW(window), "JoyCar");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //gtk_window_set_default_size(GTK_WINDOW(window), 320, 480);

    g_signal_connect_swapped(G_OBJECT(window), "destroy",
            G_CALLBACK(gtk_main_quit), G_OBJECT(window));

    main_vbox = gtk_vbox_new(FALSE, MAIN_VBOX_SPACING);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    /* motor control panel */
    motor_frame = gtk_frame_new("Motor");
    gtk_box_pack_start(GTK_BOX(main_vbox), motor_frame, TRUE, TRUE, 0);

    motor_btn_table = gtk_table_new(3, 2, TRUE);
    gtk_table_set_row_spacings(GTK_TABLE(motor_btn_table), CTRL_BTN_ROW_SPACING);
    gtk_table_set_col_spacings(GTK_TABLE(motor_btn_table), CTRL_BTN_COL_SPACING);
    gtk_container_add(GTK_CONTAINER(motor_frame), motor_btn_table);

    /* motor forward */
    motor_forward_btn = gtk_button_new_with_label("Forward");
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_forward_btn,
            0, 1, 0, 1);
    g_signal_connect(G_OBJECT(motor_forward_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_MOTOR_FORWARD);
    g_signal_connect(G_OBJECT(motor_forward_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_MOTOR_FORWARD);

    /* motor backward */
    motor_backward_btn = gtk_button_new_with_label("Backward");
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_backward_btn,
            0, 1, 1, 2);
    g_signal_connect(G_OBJECT(motor_backward_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_MOTOR_BACKWARD);
    g_signal_connect(G_OBJECT(motor_backward_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_MOTOR_BACKWARD);

    /* motor turn left */
    motor_turnleft_btn = gtk_button_new_with_label("TurnLeft");
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_turnleft_btn,
            1, 2, 0, 1);
    g_signal_connect(G_OBJECT(motor_turnleft_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_MOTOR_TURNLEFT);
    g_signal_connect(G_OBJECT(motor_turnleft_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_MOTOR_TURNLEFT);

    /* motor turn right */
    motor_turnright_btn = gtk_button_new_with_label("TurnRight");
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_turnright_btn,
            1, 2, 1, 2);
    g_signal_connect(G_OBJECT(motor_turnright_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_MOTOR_TURNRIGHT);
    g_signal_connect(G_OBJECT(motor_turnright_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_MOTOR_TURNRIGHT);

    /* motor speed */
    motor_speed_label = gtk_label_new("Motor Speed");
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_speed_label,
            0, 1, 2, 3);
    
    motor_speed_spin_adj = gtk_adjustment_new(1.0, 0.0, 3.0, 1.0, 1.0, 1.0);
    motor_speed_spin = gtk_spin_button_new(motor_speed_spin_adj, 0, 0);
    gtk_table_attach_defaults(GTK_TABLE(motor_btn_table), 
            motor_speed_spin,
            1, 2, 2, 3);

    /* holder control panel */
    holder_frame = gtk_frame_new("Holder");
    gtk_box_pack_start(GTK_BOX(main_vbox), holder_frame, TRUE, TRUE, 0);

    holder_btn_table = gtk_table_new(3, 2, TRUE);
    gtk_table_set_row_spacings(GTK_TABLE(holder_btn_table), CTRL_BTN_ROW_SPACING);
    gtk_table_set_col_spacings(GTK_TABLE(holder_btn_table), CTRL_BTN_COL_SPACING);
    gtk_container_add(GTK_CONTAINER(holder_frame), holder_btn_table);

    /* holder turn up */
    holder_turnup_btn = gtk_button_new_with_label("TurnUp");
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_turnup_btn,
            0, 1, 0, 1);
    g_signal_connect(G_OBJECT(holder_turnup_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_HOLDER_TURNUP);
    g_signal_connect(G_OBJECT(holder_turnup_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_HOLDER_TURNUP);

    /* holder turn down */
    holder_turndown_btn = gtk_button_new_with_label("TurnDown");
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_turndown_btn,
            0, 1, 1, 2);
    g_signal_connect(G_OBJECT(holder_turndown_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_HOLDER_TURNDOWN);
    g_signal_connect(G_OBJECT(holder_turndown_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_HOLDER_TURNDOWN);

    /* holder turn left */
    holder_turnleft_btn = gtk_button_new_with_label("TurnLeft");
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_turnleft_btn,
            1, 2, 0, 1);
    g_signal_connect(G_OBJECT(holder_turnleft_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_HOLDER_TURNLEFT);
    g_signal_connect(G_OBJECT(holder_turnleft_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_HOLDER_TURNLEFT);

    /* holder turn right */
    holder_turnright_btn = gtk_button_new_with_label("TurnRight");
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_turnright_btn,
            1, 2, 1, 2);
    g_signal_connect(G_OBJECT(holder_turnright_btn), "pressed",
            G_CALLBACK(btn_pressed_handler), (gpointer)ID_HOLDER_TURNRIGHT);
    g_signal_connect(G_OBJECT(holder_turnright_btn), "released",
            G_CALLBACK(btn_released_handler), (gpointer)ID_HOLDER_TURNRIGHT);

    /* holder speed */
    holder_speed_label = gtk_label_new("Holder Speed");
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_speed_label,
            0, 1, 2, 3);
    
    holder_speed_spin_adj = gtk_adjustment_new(1.0, 0.0, 3.0, 1.0, 1.0, 1.0);
    holder_speed_spin = gtk_spin_button_new(holder_speed_spin_adj, 0, 0);
    gtk_table_attach_defaults(GTK_TABLE(holder_btn_table), 
            holder_speed_spin,
            1, 2, 2, 3);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
