# Målepinde og afgrænsning for BME280-wrapperen

Den udleverede wrapper er valgt, fordi hovedopgaven i faget er at anvende
Linux på en embedded platform og bygge et fungerende IoT/IIoT-system — ikke at
alle elever skal udvikle et sensor-driver/platformslag fra bunden.

Wrapperen ændrer **ikke** på, at eleven selv skal arbejde med Linux, hardware,
C++-applikationsudvikling og IoT-integration.

## Relevante målepinde

I denne del af forløbet bidrager arbejdet især til, at eleven kan:

- anvende almindelige Shell-kommandoer på det Linux-baserede system,
- installere software via Shell,
- forholde sig til bruger-/grupperettigheder til Linux devices,
- udvikle applikation på et Linux-baseret embedded system,
- opbygge et IoT/IIoT-system der indlæser sensordata, behandler dem og
  videresender dem til et centralt system.

## Hvad er udleveret?

```text
BME280 abstraction
Bosch ↔ Linux callback-integration
Linux i2c-dev transportkode
```

Kildekoden er synlig og skal kunne forklares overordnet.

## Hvad er stadig elevens arbejde?

```text
fysisk tilslutning
Linux bus/device discovery
i2cdetect
sensoradresse
chip-ID verifikation
rettigheder
Sensor Publisher design
configuration
JSON
genbrug af MQTT publisher
fejlhåndtering
Makefile/projektintegration
MQTT topic
integration med Telemetry Processor
integration med resten af IoT-kæden
```

Det betyder, at wrapperen kun abstraherer den specialiserede del:

```text
Bosch callback API
+
Linux I2C transaction implementation
```

Den abstraherer **ikke** Linux-systemet eller IoT-applikationen væk.

## Tre niveauer

### Standardniveau

Brug den udleverede `Bme280Sensor`, og udvikl resten af Sensor Publisher selv.

### Forståelsesniveau

Kunne følge og forklare:

```text
Bme280Sensor::read()
→ Bosch SensorAPI
→ callback
→ Linux I2C
→ /dev/i2c-N
→ BME280
```

### Udvidelsesniveau

Implementér selv platformlaget ud fra Bosch SensorAPI og Linux I2C
dokumentationen.

Dermed kan elever, der har overskud, gå dybere i embedded Linux uden at denne
specialiserede detalje bliver en blokering for alle andre grupper.
