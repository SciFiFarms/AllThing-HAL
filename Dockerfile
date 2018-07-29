FROM sglahn/platformio-core
COPY ./ /workspace/
VOLUME /workspace
RUN pip install -U paho-mqtt==1.3 hvac==0.2.17 ptvsd==3.0.0 platformio==3.5.3
ENTRYPOINT ["python", "entrypoint.py"]