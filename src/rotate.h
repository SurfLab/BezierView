#ifndef ROTATE_H
#define ROTATE_H

typedef enum KeyboardModifier {
    NoModifier           = 0x00000000,
    ShiftModifier        = 0x02000000,
    ControlModifier      = 0x04000000,
    AltModifier          = 0x08000000,
    MetaModifier         = 0x10000000,
    KeypadModifier       = 0x20000000,
    GroupSwitchModifier  = 0x40000000,
} KeyboardModifier;

void mouseMotion(int x, int y, KeyboardModifier modifiers );
void mouseButton(int button, int state, int x, int y, KeyboardModifier modifiers);
void display();
void spin();
void zoom(float delta);
extern int clip_item;

#endif // ROTATE_H

