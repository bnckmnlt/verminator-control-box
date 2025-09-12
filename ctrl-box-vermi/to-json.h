#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <ArduinoJson.h>

void createBeddingJson(JsonObject obj, float temp, float moist, float soilMoist);
void createCompostJson(JsonObject obj, float nitrogen, float phosphorus, float potassium, float compostWeight);
void createFluidsJson(JsonObject obj, float juiceWeight, float waterWeight);
String formatTemplate(String input);

#endif