#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

// WiFi Credentials
#define WIFI_SSID "Mrudu's Data"
#define WIFI_PASSWORD "notforuh"

// Firebase Credentials
#define API_KEY "AIzaSyD6WCmaxM8XdY5Wzq1xQ-sXO-T0rh68P6o"
#define DATABASE_URL "https://smartshop-8ac58-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "mrudu20045d@gmail.com"
#define USER_PASSWORD "smartshop"

// RFID Pins
#define SS_PIN 21
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int totalAmount = 0, totalItems = 0;

struct Product {
  String uid;
  String name;
  int price;
  int count;
};

// Product List
Product products[] = {
  {"46462803", "Chocolate", 10, 0},
  {"5704C201", "Ice Cream", 100, 0}
};

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected!");

  // Connect to Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Scan a product...");
  Serial.println();
  Serial.println("QR Code for: Smart Shopping");
  Serial.println();

   Serial.println("                                                              ");
  Serial.println("  ██████████████  ████  ██  ████  ██  ████    ██████████████  ");
  Serial.println("  ██          ██        ██  ██████    ██  ██  ██          ██  ");
  Serial.println("  ██  ██████  ██    ████  ██  ██    ████████  ██  ██████  ██  ");
  Serial.println("  ██  ██████  ██  ██    ████        ██  ████  ██  ██████  ██  ");
  Serial.println("  ██  ██████  ██  ████              ██  ████  ██  ██████  ██  ");
  Serial.println("  ██          ██  ████  ██        ████  ██    ██          ██  ");
  Serial.println("  ██████████████  ██  ██  ██  ██  ██  ██  ██  ██████████████  ");
  Serial.println("                  ████████  ██  ████                          ");
  Serial.println("  ██      ██  ██████    ██  ██          ██████████████    ██  ");
  Serial.println("  ██████████    ██      ██  ████  ██  ████    ██  ██████████  ");
  Serial.println("      ████  ██████████████        ████  ██      ██        ██  ");
  Serial.println("  ██  ██  ████  ████      ██  ██    ██████████  ██      ████  ");
  Serial.println("  ██    ██    ██  ████  ██  ████████  ██  ██    ██      ██    ");
  Serial.println("  ██    ██████      ████████        ██  ████████  ████  ████  ");
  Serial.println("  ████████    ██  ██  ████████████    ██  ██████████  ██  ██  ");
  Serial.println("  ████████  ██    ████      ██  ████    ████            ████  ");
  Serial.println("    ██    ██████  ██        ██            ██    ██  ██  ██    ");
  Serial.println("  ████  ██████  ██████      ████  ██  ██  ██████  ██    ████  ");
  Serial.println("      ██  ██  ██    ██████        ████    ██████████████  ██  ");
  Serial.println("          ██    ██            ██    ████  ██████  ██    ████  ");
  Serial.println("  ██████████  ██    ████████████████  ████████████████    ██  ");
  Serial.println("                  ██    ████        ██    ██      ██      ██  ");
  Serial.println("  ██████████████  ██  ████  ██████    ██████  ██  ██████  ██  ");
  Serial.println("  ██          ██            ██  ████      ██      ██          ");
  Serial.println("  ██  ██████  ██  ████  ██████          ████████████          ");
  Serial.println("  ██  ██████  ██    ██      ████  ██  ██  ██        ██████    ");
  Serial.println("  ██  ██████  ██      ████████    ████    ████  ██  ██  ████  ");
  Serial.println("  ██          ██    ██████    ██    ████████      ██    ████  ");
  Serial.println("  ██████████████  ██          ██████  ██████  ██  ██    ██    ");
  Serial.println("                                                              ");
  Serial.println("\n✅ Done! Scan this QR code with your phone.");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
  Serial.println("Scanned UID: " + uidStr);

  bool found = false;
  for (auto &p : products) {
    if (p.uid == uidStr) {
      found = true;
      p.count++;

      if (p.count % 2 == 1) {
        totalItems++;
        totalAmount += p.price;
        Serial.println(p.name + " added! Rs." + String(p.price));
      } else {
        totalItems--;
        totalAmount -= p.price;
        Serial.println(p.name + " removed! Rs." + String(p.price));
      }

      // Update Firebase product details
      if (Firebase.ready()) {
        String path = "/cart/products/" + p.uid;
        if (p.count % 2 == 1) {  // If added
          Firebase.setString(fbdo, path + "/name", p.name);
          Firebase.setInt(fbdo, path + "/price", p.price);
          Firebase.setInt(fbdo, path + "/count", p.count / 2 + 1);
        } else {  // If removed
          if (p.count == 0) {
            Firebase.deleteNode(fbdo, path);  // Remove from database if count reaches 0
          } else {
            Firebase.setInt(fbdo, path + "/count", p.count / 2);
          }
        }
        Firebase.setInt(fbdo, "/cart/totalItems", totalItems);
        Firebase.setInt(fbdo, "/cart/totalAmount", totalAmount);
      }

      // Reset cart if all items are removed
      if (totalItems <= 0) {
        resetCart();
        return;
      }
      break;
    }
  }

  if (!found) {
    Serial.println("Unknown Product!");
  }

  delay(1500);
}

// Function to reset cart
void resetCart() {
  Serial.println("Cart reset!");

  for (auto &p : products) {
    p.count = 0;
  }

  totalItems = 0;
  totalAmount = 0;

  if (Firebase.ready()) {
    Firebase.setInt(fbdo, "/cart/totalItems", totalItems);
    Firebase.setInt(fbdo, "/cart/totalAmount", totalAmount);
    Firebase.setString(fbdo, "/cart/message", "Lets shop!");
    Firebase.deleteNode(fbdo, "/cart/products");  // Clear product list
  }
}