#include "to-json.h"

void createBeddingJson(JsonObject obj, float temp, float moist, float soilMoist) {
  JsonObject temperature = obj.createNestedObject("temperature");
  temperature["value"] = temp;
  temperature["unit"] = "Â°C";

  JsonObject humidity = obj.createNestedObject("humidity");
  humidity["value"] = moist;
  humidity["unit"] = "%";

  JsonObject soil_moisture = obj.createNestedObject("soil_moisture");
  soil_moisture["value"] = soilMoist;
  soil_moisture["unit"] = "%";
}

void createCompostJson(JsonObject obj, float nitrogen, float phosphorus, float potassium, float nitrogen_agro_pct, float phosphorus_agro_pct, float potassium_agro_pct, float compostWeight) {
  JsonObject npk = obj.createNestedObject("npk");
  npk["nitrogen"] = nitrogen;
  npk["phosphorus"] = phosphorus;
  npk["potassium"] = potassium;
  npk["nitrogen_agro_pct"] = nitrogen_agro_pct;
  npk["phosphate_agro_pct"] = phosphorus_agro_pct;
  npk["potash_agro_pct"] = potassium_agro_pct;
  npk["unit"] = "%";

  JsonObject compost_weight = obj.createNestedObject("compost_weight");
  compost_weight["value"] = compostWeight;
  compost_weight["unit"] = "Kilo";
}

void createFluidsJson(JsonObject obj, float juiceWeight, float waterWeight) {
  JsonObject juice_weight = obj.createNestedObject("juice_weight");
  juice_weight["value"] = juiceWeight;
  juice_weight["unit"] = "Liters";

  JsonObject water_weight = obj.createNestedObject("reservoir_weight");
  water_weight["value"] = waterWeight;
  water_weight["unit"] = "Liters";
}

String formatTemplate(String input) {
  return "<" + input + ">";
}
