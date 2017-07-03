
// Time to wait for key debounce
#define DEBOUNCE 1

// Number of columns on the keyboard matrix (highest if columns are not equal)
#define NUM_OF_COLS 14
// Number of rows on the keyboard matrix (highest if rows are not equal)
#define NUM_OF_ROWS 6

// Pin wired to caps lock LED. -1 if no LED is present.
#define CAPS_LOCK_PIN 3
// Pin wired to scroll lock LED. -1 if no LED is present.
#define SCROLL_LOCK_PIN -1
// Pin wired to num lock LED. -1 if no LED is present.
#define NUM_LOCK_PIN -1

// Pin numbers for columns in order they are wired with respect to keyMap. FYI: Column pins are INPUT_PULLDOWN
int cols[NUM_OF_COLS] =  {23,22,21,20,19,18,17,16,15,14,13,12,11,10};
// Pin numbers for rows in order they are wired with respect to keyMap. FYI: Row pins are INPUT_PULLUP
int rows[NUM_OF_ROWS] = {4,5,6,7,8,9};

// The order that buttons are wired. FYI, keyMap[0][0] will activated if pins at cols[0], rows[0] are on.
int keyMap[NUM_OF_ROWS][NUM_OF_COLS] = 
{
  { KEY_ESC,       KEY_F1,        KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,    KEY_F10,    KEY_F11,        KEY_F12,         0 },
  { KEY_CAPS_LOCK, KEY_1,         KEY_2,  KEY_3,  KEY_4,  KEY_5,  KEY_6,  KEY_7,  KEY_8,  KEY_9,     KEY_0,      KEY_MINUS,      KEY_EQUAL,       KEY_PRINTSCREEN },
  { KEY_TILDE,     KEY_Q,         KEY_W,  KEY_E,  KEY_R,  KEY_T,  KEY_Y,  KEY_U,  KEY_I,  KEY_O,     KEY_P,      KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_PAUSE},
  { KEY_HOME,      KEY_BACKSLASH, KEY_A,  KEY_S,  KEY_D,  KEY_F,  KEY_G,  KEY_H,  KEY_J,  KEY_K,     KEY_L,      KEY_SEMICOLON,  KEY_QUOTE,       KEY_ENTER },
  { KEY_END,       KEY_INSERT,    KEY_Z,  KEY_X,  KEY_C,  KEY_V,  KEY_B,  KEY_N,  KEY_M,  KEY_COMMA, KEY_PERIOD, KEY_SLASH,      KEY_UP,          0 },
  { KEY_PAGE_UP, MODIFIERKEY_SHIFT, MODIFIERKEY_CTRL, MODIFIERKEY_GUI, KEY_TAB, KEY_SPACE, MODIFIERKEY_ALT, KEY_BACKSPACE, KEY_DELETE, 101, KEY_PAGE_DOWN, KEY_LEFT, KEY_DOWN, KEY_RIGHT }
};


// Blow this line is "unconfigurable"


// 2D array to keep track of which keys are pressed
int** keyState;

// Storage for which key is pressed in which "slot"
int keys[6] {0, 0, 0, 0, 0, 0};

// Storage for pressed modifiers.
int modifiers[4] {0, 0, 0, 0};
// Indexs for modifier keys in modifiers buffer
#define SHIFT 0
#define CTRL 1
#define ALT 2
#define GUI 3

// LED numbers for usb_keyboard.h
#define USB_LED_NUM_LOCK 0
#define USB_LED_CAPS_LOCK 1
#define USB_LED_SCROLL_LOCK 2
#define USB_LED_COMPOSE 3
#define USB_LED_KANA 4

void setup() {
  // Turn on LEDs for output
  if(CAPS_LOCK_PIN >= 0)
  {
    pinMode(CAPS_LOCK_PIN, OUTPUT);
  }
  if(SCROLL_LOCK_PIN >= 0)
  {
    pinMode(SCROLL_LOCK_PIN, OUTPUT);
  }
  if(NUM_LOCK_PIN >= 0)
  {
    pinMode(NUM_LOCK_PIN, OUTPUT);
  }


  // Create and initialize keyState off rows by columns.
  keyState = (int**)malloc(NUM_OF_ROWS * sizeof(int*)); 
  for (int i = 0; i < NUM_OF_ROWS; i++) {
       keyState[i] = (int *)malloc(NUM_OF_COLS * sizeof(int));
  }
  for (int i = 0; i < NUM_OF_ROWS; i++) {
    for(int c = 0; c < NUM_OF_COLS; c++)
    {
      keyState[i][c] = LOW;
    }
  }
  
}

// Read the column, HIGH if on LOW of off.
int ReadColumn(int index)
{
  for(int i = 0; i < NUM_OF_ROWS; i++)
  {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
  }
  pinMode(cols[index], INPUT_PULLDOWN);
  delay(DEBOUNCE);
  return digitalRead(cols[index]);
}

// Checks for an open button to send
int CheckForButton()
{
    for(int i = 0; i < 6; i++){
      if(keys[i] == 0) return i;
    }
    return -1;
}

// Sets the modifier buffer if a modifier key was pressed.
bool SetModifier(int b)
{
  if(b == MODIFIERKEY_SHIFT)
  {
    modifiers[SHIFT] = MODIFIERKEY_SHIFT;
    return true;
  }
  else if (b == MODIFIERKEY_CTRL)
  {
    modifiers[CTRL] = MODIFIERKEY_CTRL;
    return true;
  }
  else if (b == MODIFIERKEY_ALT)
  {
    modifiers[ALT] = MODIFIERKEY_ALT;
    return true;
  }
  else if (b == MODIFIERKEY_GUI)
  {
    modifiers[GUI] = MODIFIERKEY_GUI;
    return true;
  }
  return false;
}

// Unsets a modifer button from the buffer.
bool UnsetModifier(int b)
{
    if(b == MODIFIERKEY_SHIFT)
  {
    modifiers[SHIFT] = 0;
    return true;
  }
  else if (b == MODIFIERKEY_CTRL)
  {
    modifiers[CTRL] = 0;
    return true;
  }
  else if (b == MODIFIERKEY_ALT)
  {
    modifiers[ALT] = 0;
    return true;
  }
  else if (b == MODIFIERKEY_GUI)
  {
    modifiers[GUI] = 0;
    return true;
  }
  return false;
}

// Ors all modifiers and sets them on the keyboard.
void SendModifiers()
{
  int m = 0;
  for(int i = 0; i < 4; i++)
  {
    m |= modifiers[i];
  }
  Keyboard.set_modifier(m);
}

// Checks to see if a button is pressed in the buffer
int CheckForPressedButton(int b)
{
  for(int i = 0; i < 6; i++){
    if(keys[i] == b)
    { 
      return i;
    }
  }
  return -1;
}

// Sets the key based off of the position in the buffer
void PressButton(int index, int b){
  keys[index] = b;
    switch(index + 1){
      case 1:
        Keyboard.set_key1(b);
        break;
      case 2:
        Keyboard.set_key2(b);
        break;
      case 3:
        Keyboard.set_key3(b);
        break;
      case 4:
        Keyboard.set_key4(b);
        break;
      case 5:
        Keyboard.set_key5(b);
        break;
      case 6:
        Keyboard.set_key6(b);
        break;
    
  }
}

// Unpresses a key based off of its position in the buffer
void DepressButton(int index){
  keys[index] = 0;
  switch(index + 1){
    case 1:
      Keyboard.set_key1(0);
      break;
    case 2:
      Keyboard.set_key2(0);
      break;
    case 3:
      Keyboard.set_key3(0);
      break;
    case 4:
      Keyboard.set_key4(0);
      break;
    case 5:
      Keyboard.set_key5(0);
      break;
    case 6:
      Keyboard.set_key6(0);
      break;
  }
}

// Checks to see if LED lights should be on or off.
void ToggleLeds()
{
  if(CAPS_LOCK_PIN >= 0 && keyboard_leds & (1<<USB_LED_CAPS_LOCK))
  {
    digitalWrite(CAPS_LOCK_PIN, HIGH);
  }
  else
  {
    digitalWrite(CAPS_LOCK_PIN, LOW);
  }
  
  if(NUM_LOCK_PIN >= 0 && keyboard_leds & (1<<USB_LED_NUM_LOCK))
  {
    digitalWrite(NUM_LOCK_PIN, HIGH);
  }
  else
  {
    digitalWrite(NUM_LOCK_PIN, LOW);
  }
  
  if(SCROLL_LOCK_PIN >= 0 && keyboard_leds & (1<<USB_LED_NUM_LOCK))
  {
    digitalWrite(SCROLL_LOCK_PIN, HIGH);
  }
  else
  {
    digitalWrite(SCROLL_LOCK_PIN, LOW);
  }
}

// Test a key to see if it was just depressed.
void TryKeyDepressed(int r, int c, int readRow)
{
  if(readRow == HIGH && keyState[r][c] == HIGH)
  {
    keyState[r][c] = LOW;
    if(!UnsetModifier(keyMap[r][c]))
    {
      int b = CheckForPressedButton(keyMap[r][c]);
      if(b >= 0)
      {
        //digitalWrite(ledPin,LOW); 
        DepressButton(b);
      }
    }
  }
}

// Test a key to see if it was just pressed.
bool TryKeyPressed(int r, int c, int readRow)
{
  if(readRow == LOW && keyState[r][c] == LOW)
  {
    keyState[r][c] = HIGH;
    if(!SetModifier(keyMap[r][c]))
    {
      int b = CheckForButton();
      if(b >= 0)
      {
        //digitalWrite(ledPin,HIGH); 
        PressButton(b, keyMap[r][c]);
      }
    }
    return true;
  }
  return false;
}

void loop() {
  for(int c = 0; c < NUM_OF_COLS; c++)
  {
    int readC = ReadColumn(c);

    // Turn column read off
    pinMode(cols[c], OUTPUT_OPENDRAIN);
    digitalWrite(cols[c], LOW);
    delay(DEBOUNCE);
    
    for(int r = 0; r < NUM_OF_ROWS; r++)
    {
      // Turn on row for reading
      pinMode(rows[r], INPUT_PULLUP);
      
      int readRow = digitalRead(rows[r]);
      if(readC == HIGH)
      {
        if(!TryKeyPressed(r, c, readRow))
        {
          TryKeyDepressed(r, c, readRow);
        }
      }
      else
      {
        TryKeyDepressed(r, c, readRow);
      }
      // Turn row read off.
      pinMode(rows[r], OUTPUT);
      digitalWrite(rows[r], HIGH);
    }
    // Turn column write off.
    pinMode(cols[c], INPUT_PULLDOWN);
  }

  SendModifiers();
  ToggleLeds();
  Keyboard.send_now();
}
