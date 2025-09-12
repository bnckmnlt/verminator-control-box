#include "scale.h"
#include "npk.h"
#include "env.h"
#include "soil.h"
#include "relay.h"
#include "to-json.h"
#include "aeration.h"
#include "sifter.h"
#include "serial.h"
#include "misting.h"

unsigned long baudRate = 115200;

const double compostContainerCap = 48.0;
const double vermiteaContainerCap = 28.0;
const double reservoirContainerCap = 28.0;

/** Timings **/
unsigned long currentMillis;
unsigned long sensorReadingMillis;
unsigned long scaleLvlMillis;
unsigned long scaleCalMillis;
const unsigned long sensorReadingPeriod = 10000;
const unsigned long scaleLvlPeriod = 1000;
const unsigned long scaleCalPeriod = 500;

void setup()
{
  Serial.begin(baudRate);

  if (!env::begin())
  {
    Serial.println("<error: Could not find a valid BME280 sensor, check wiring!>");
  }

  soil::begin();
  npk::setup();
  scale::begin();
  relay::begin();

  Serial.println("<info:[Mega] Board initialized>");
}

void loop()
{
  currentMillis = millis();
  recvWithStartEndMarkers();
  processSerialData();

  if (currentMillis - sensorReadingMillis >= sensorReadingPeriod)
  {
    sendtoSerial();

    sensorReadingMillis = millis();
  }

  if (currentMillis - scaleLvlMillis >= scaleLvlPeriod)
  {
    env::read();
    npk::read();
    soil::read();

    scale::read(scaleCompost, scale::compostKilo, compostContainerCap, "WL:Compost");
    scale::read(scaleJuice, scale::juiceLiters, vermiteaContainerCap, "WL:Juice");
    scale::read(scaleWater, scale::waterLiters, reservoirContainerCap, "WL:Reservoir");

    soil::control();
    env::control();
    aeration::control();

    misting::control();

    scaleLvlMillis = millis();
  }

  if ((currentMillis - scaleCalMillis >= scaleCalPeriod) && (scale::scaleCalMode == true))
  {
    scale::calibrate(scale::calibrationId);
  }
}

void sendtoSerial()
{
  StaticJsonDocument<512> doc;

  JsonObject layers = doc.createNestedObject("layers");
  createBeddingJson(layers.createNestedObject("bedding"), env::temperature, env::humidity, soil::moisture);
  createCompostJson(layers.createNestedObject("compost"), npk::nitrogen, npk::phosphorus, npk::potassium, scale::compostKilo);
  createFluidsJson(layers.createNestedObject("fluid"), scale::juiceLiters, scale::waterLiters);

  Serial.print("<Payload:");
  serializeJson(doc, Serial);
  Serial.println(">");
}
