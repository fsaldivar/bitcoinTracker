#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Configuracion del Internet
const char ssid[] = "AXTEL XTREMO-EF67";
const char password[] = "036AEF67";

//Declaramos el pin de salida y el numero de LEDs del NeoPixel
#define PIN 12  
#define NUMPIXELS 16

//OLED
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2

// Previous Bitcoin value & threshold
float previousValue = 0.0;
float threshold = 0.05;
float calc=0.0;
int  n=0;

//arrancamos el circulo de leds
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//arrancamos la pantalla LED

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


//Colores de los LEDS
uint32_t redColor = pixels.Color(255,0,0,127);
uint32_t greenColor = pixels.Color(0,255,0,255);
uint32_t blueColor = pixels.Color(0,0,255,255);
uint32_t resetColor = pixels.Color(0,0,0);



// API server
const char* host = "api.coindesk.com";


void setup() {

  // Serial
  Serial.begin(115200);
  pixels.begin();
  delay(10);
  pixels.show();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  // Clear the buffer.
  display.display();
  display.clearDisplay();
  display.display();

   //tex display test
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // Connect to API
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/v1/bpi/currentprice.json";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Read all the lines of the reply from server and print them to Serial
  String answer;
  while(client.available()){
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();
  Serial.println();
  Serial.println("closing connection");

  // Process answer
  Serial.println();
  Serial.println("Answer: ");
  Serial.println(answer);

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Get JSON data
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
  priceString.trim();
  float price = priceString.toFloat();

  // Print price
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.println(price);
 
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("El precio de Bitcoin es: $");
  display.print(price);
  display.print(" USD");
  display.display();
 // Init previous value 
  if (previousValue == 0.0) {
    previousValue = price;
    

    
  }


// No Alert ?
if (price == (previousValue)) {
    Serial.print("Precio Anterior:");
    Serial.println(previousValue);
    Serial.print("Precio Nuevo");
    Serial.println(price);
    Serial.print("El precio es el mismo");
    Serial.println();
    // Flash LED
//pixels.setPixelColor(1, pixels.Color(255,0,0));

  pixels.fill(blueColor, 0, 1);
  pixels.show();    
  }




// Alert down ?
if ((price < previousValue) && (price > 0)) {
    calc = (1-(price/previousValue))*100;
    Serial.print("Precio Anterior:");
    Serial.println(previousValue);
    Serial.print("Precio Nuevo");
    Serial.println(price);
    Serial.print("El porcentaje de decremento es:");
    Serial.println(calc);
    // Flash LED
//pixels.setPixelColor(1, pixels.Color(255,0,0));
  
  
  
  ledCondition(calc);
  pixels.fill(redColor, 0, n);
  pixels.show();    
  }

// Alert up ?
  if (price > (previousValue)) {
    calc = ((price/previousValue)-1)*100;
    Serial.print("Precio Anterior:");
    Serial.println(previousValue);
    Serial.print("Precio Nuevo");
    Serial.println(price);
    Serial.print("El porcentaje de incremento es:");
    Serial.println(calc);

// Flash LED
//pixels.setPixelColor(1, pixels.Color(0,255,0));
    
    ledCondition(calc);
    
  pixels.fill(greenColor, 0, n);
    pixels.show();    
    
  }




  // Store value
  previousValue = price;


  // Wait 5 seconds
  delay(30000);

  //Reseteo
  pixels.fill(resetColor, 0, 16);

}




void ledCondition(float calc){

//Condiccion de proteccion por si la API manda un 0
   if (calc > 90.0) {
    n =1;
  }
  
  if (calc > 1) {
    n =16;
  }
  
  else if (calc > 0.85) {
    n =14;
  }
  
  else if (calc > 0.65) {
    n =12;
  }
  
   else if (calc > 0.55) {
    n =10;
  }
  
  else if (calc > 0.5) {
    n =8;
  }
  
  else if (calc > 0.35) {
    n =6;
  }
  
  else if (calc > 0.25) {
    n =4;
  }
  
   else if (calc > 0.15) {
    n =2;
  }
  
    
  else {
    n=1;
  }
 
  return ;
  
}


