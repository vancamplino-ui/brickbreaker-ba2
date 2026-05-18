#ifndef GRAPHIC_H
#define GRAPHIC_H

enum Color {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PURPLE,
    BLACK,
    GREY,
    WHITE,
};

// Prototypes des fonctions de dessin
void graphic_draw_rectangle(double x_center, double y_center, 
                            double size, Color color); 
void graphic_draw_circle(double x_center, double y_center, 
                         double radius, Color color);    
void graphic_draw_arc(double x_center, double y_center, 
                      double radius, Color color);
void graphic_draw_border(double x_center, double y_center, 
                         double radius, Color borderColor); 

#endif
