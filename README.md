# Arduino_irrigation_water_analysis_system_using_LoRaWan
Irrigation water analysis equipment passing through the growing medium. Vertical farming. Kasvien kastelussa kierrätetyn veden analysointijärjestelmä.
Tämä on avoimen lähdekoodin IoT-projekti, joka mittaa kasvihuoneessa/tunneliviljelyssä linjaston läpi kulkeneen veden määrää, johtavuutta ja pH:ta käyttäen akkukäyttöistä Arduino MKR WAN 1310-solmua. Data lähetetään pitkän kantaman LoRaWAN-verkon ja Dragino-reitittimen kautta etähallittavaan Raspberry Pi:n InfluxDB-aikasarjatietokantaan.

## 🚀 Arkkitehtuuri

### End Node (Arduino) --(LoRa 868MHz)--> Gateway (Dragino) --> Backend (InfluxDB)
### Mittaussolmu (Arduino MKR) --> (LoRa 868MHz)--> Gateway (Dragino) --> ChirpStack --> Backend (InfluxDB)

## 📁 Repositorion rakenne 
* `/firmware` - Arduino C++ koodi (lähettävä pää) sekä veden tyhjennysventtiilin ohjaus
* `/gateway` - Ohjeet ja asetukset Dragino-reitittimen konfigurointiin
* `/backend` - InfluxDB-tietokannan asennusohjeet ja datan vastaanottoskriptit
* `/hardware` - Kytkentäkaaviot, osaluettelo (BOM) ja mahdolliset 3D-tulostustiedostot
## 🛠️ Laitteisto ja komponentit 
* **Mikrokontrolleri:** Arduino MKR WAN 1310 ja XIAO Esp32-C3
* **LoRa-moduuli:** Murata CMWX1ZZABZ LoRa® module (sisältyy MKR 1310, Taajuus: 868 MHz EU)
* **Anturit:** DFrobot analog pH sensor pro (SEN0169), DfRobot analog conductivity sensor (SEN0451), Adafruit StemmaQT VL53L4CD Time of Flight -etäisyysanturi 
* **Gateway:** Dragino LoRaWAN Gateway LPS8V2 *
* **Toimilaite** Servo- moottori 
* **Virtalähde:**  AKY0393Li-Po -akku 2200mAh (Arduino + sensors), ? (XIAO), ? (Servo moottori)
## 💻 Ohjelmistovaatimukset
  * Arduino IDE mittaussolmun ja tyhjennysventtiilin ohjelmointiin 
## ⚙️ Käyttöönotto-ohjeet
### 1. Laitteisto ja kytkennät: 
* Katso tarkat kytkentäkaaviot ja pinnijärjestykset kansiosta `/hardware`.
### 2. Solmun ohjelmointi (`/firmware`) 
* Avaa koodi Arduino IDE:ssä.
* Asenna tarvittavat kirjastot: `Arduino Low Power`, MKRWAN_V2 ja DFRobot_ECPRO-master, Sparkfun_VL53L1X_4m_Laser_Distance_Sensor [anturikirjastot].
* Avaa tiedosto `arduino_secrets.h` (tai vastaava) ja syötä oma LoRaWAN-avaimesi (`AppEUI`, `AppKey` Arduino MKR1310 valmiiksi ohjelmoituna).
### 3. Dragino & LoRaWAN-palvelin (`/gateway`) 
* Konfiguroi Dragino-reititin käyttämään oikeaa taajuussuunnitelmaa (`EU868`, katso kuvankaappaukset).
* Ohjaa reititin LoRaWAN-palvelimelle (tässä tapauksessa ChirpStack-palvelin Draginon sisällä).
* Lisää `/backend/decoder.js` -tiedoston sisältö LoRaWAN-palvelimen *Payload Formatter* -osioon, jotta tavumuotoinen data muuttuu JSON-muotoon.
### 4. Tietokanta (`/backend`) 
* Tietokantaa ajetaan Raspberry Pi 5:ssä, mikä tekee asennuksesta vaivatonta. 
* Asenna Influxdb -tietokanta ohjeiden mukaisesti
* Käynnistä tietokanta komennolla: ```???? ```
## 🔒 Tietoturva & Etäyhteys
* Jos asennat etäyhteyden, etäyhteys InfluxDB-tietokantaan on suojattava. Älä avaa tietokantaportteja suoraan julkiseen internetiin. Suositellut tavat etäyhteyden muodostamiseen: *
*  Käytä RPI omaa etäyhteyttä Raspberry Pi Connect**
*  Tailscale / WireGuard (Suositeltu myös):** Luo suojattu VPN-yhteys laitteidesi välille ilman avoimia portteja. 
    
    * ### 🤝 Osallistuminen, parannusehdotukset ja virheilmoitukset ovat tervetulleita!
