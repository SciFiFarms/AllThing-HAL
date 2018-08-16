FROM sglahn/platformio-core
COPY ./ /workspace/
RUN pip install -U platformio
RUN pip install -U paho-mqtt==1.3 hvac==0.2.17 ptvsd==3.0.0 platformio==3.5.3
RUN pio platform install espressif8266 --with-package framework-arduinoespressif8266 \
    && pio platform install espressif32 \ 
    && pio lib install "Adafruit Unified Sensor" \
    && pio lib install "DHT sensor library" 
# TODO: I should be able to get this running. It'll save some time building ESPs.
#RUN ["platformio", "run" , "--environment", "nodemcuv2"]
#RUN pio lib install git+https://github.com/SciFiFarms/AllThing-Homie.git#v2.1 
RUN mkdir -p /workspace/data/homie
VOLUME /workspace
ENTRYPOINT ["python", "entrypoint.py"]