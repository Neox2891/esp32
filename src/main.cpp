#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LED 2

//para evitar que el dhcp nos asigne ip, o si el ruter no cuenta con dhcp
//podemos seleccionar una ip fija si no lo usas comentar las 5 líneas
// IPAddress local_IP(192, 168, 0, 184);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);

const char *ssid = "Estrada";
const char *password = "unionenlaA";

//*****************************
//***   CONFIGURACION MQTT  ***
//*****************************

const char *mqtt_server = "13.58.59.55";
const int mqtt_port = 1883;
// const char *mqtt_user = "web_client";
// const char *mqtt_pass = "121212";

long lastMsg = 0;
char msg[25];

WiFiClient espClient;
PubSubClient client(espClient);

// String setup_wifi();

String setup_wifi(const char *ssid, const char *pwd)
{

    delay(10);

    Serial.println();
    Serial.print("Conectando a: ");
    Serial.print(ssid);

    WiFi.begin(ssid, pwd);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }

    Serial.println();
    Serial.println("Conectado a la red Wifi!!!!");
    Serial.println("Direccion IP: ");
    Serial.println(WiFi.localIP());

    return "Connected!!!";
}

void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void setup()
{
    // Set pin mode
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    String connect = setup_wifi(ssid, password);
    Serial.print(connect);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
    long now = millis();

    if (now - lastMsg > 10000)
    {
        lastMsg = now;
        String to_send = "34,45,43";
        to_send.toCharArray(msg, 25);
        Serial.print("Publish message -> ");
        Serial.println(msg);
        client.publish("testtopic", msg);
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String incoming = "";

    Serial.print("Mensaje enviado desde -> ");
    Serial.print(topic);
    Serial.println("");

    for (int i = 0; i < length; i++)
    {
        incoming += (char)payload[i];
    }

    incoming.trim();
    Serial.println("Mensaje -> " + incoming);

    if (incoming == "on")
    {
        digitalWrite(LED, HIGH);
    }
    else if (incoming == "off")
    {
        digitalWrite(LED, LOW);
    }
}

void reconnect()
{

    while (!client.connected())
    {
        Serial.print("Intentando conexión Mqtt...");
        // Creamos un cliente ID
        String clientId = "esp32_";
        clientId += String(random(0xffff), HEX);
        // Intentamos conectar
        if (client.connect(clientId.c_str()))
        {
            Serial.println("Conectado!");

            // Nos suscribimos a comandos
            // char topic[25];
            // String topic_aux = serial_number + "/command";
            // topic_aux.toCharArray(topic, 25);
            // client.subscribe(topic);

            // Nos suscribimos a username
            // char topic2[25];
            // String topic_aux2 = serial_number + "/user_name";
            // topic_aux2.toCharArray(topic2, 25);
            // client.subscribe(topic2);
            client.subscribe("led1");
        }
        else
        {
            Serial.print("falló :( con error -> ");
            Serial.print(client.state());
            Serial.println(" Intentamos de nuevo en 5 segundos");

            delay(5000);
        }
    }
}