import ssl
import sys
import os
import ptvsd
import paho.mqtt.client
from string import Template

import hvac



def on_connect(client, userdata, flags, rc):
    print('connected (%s)' % client._client_id)
    client.subscribe(topic='platformio/build/*', qos=2)

def on_message(client, userdata, message):
    print('------------------------------')
    print('topic: %s' % message.topic)
    print('payload: %s' % message.payload)
    print('qos: %d' % message.qos)
    sys.stdout.flush()

    vault_client = hvac.Client(url='https://vault:8200', token=os.environ['VAULT_TOKEN'], verify='ca')
    vault_client.token = os.environ['VAULT_TOKEN']



    name = "plant_eye_001"
    # Create the vault policy. $1 = username/file prefix
    with open("plant_eye_policy.hcl", "r") as file:
        policy_template = Template(file.read())
    policy = policy_template.substitute(display_name=name)

    vault_client.set_policy(name, policy)

    # Create corresponding Rabbit MQ role
    with open("plant_eye_permissions.json", "r") as file:
        permissions_template = Template(file.read())
    permissions = permissions_template.substitute(display_name=name)
    vault_client.write("rabbitmq/roles/" + name, vhosts=permissions)

    #client.write('secret/foo', baz='bar', lease='1h')
    # Create the policy's token and make it the active token.
    # local token=$(vault_i token create -policy=$1 -ttl="1m" -field="token")
    vault_client.token = vault_client.create_token(policies=[name], display_name=name, lease='720h')['auth']['client_token']

    # Request credentials for policy. 
    # local creds=$(vault_i read -format=json rabbitmq/creds/$1)
    creds = vault_client.read("rabbitmq/creds/{0}".format(name))
    plant_eye_mqtt_username = creds['data']['username']
    plant_eye_mqtt_password = creds['data']['password']

    device_config_template= Template(message.payload)
    device_config = device_config_template.substitute(mqtt_username=plant_eye_mqtt_username, mqtt_password=plant_eye_mqtt_password)

    #print(client.read('secret/foo'))
    # Take topic, remove platformio/build/ 
    target = message.topic.replace("platformio/build/", "")
    # Use remaining as build target
    os.system('platformio run --environment ' + target)
    # Create .json file with payload in it. 
    with open('data/homie/config.json', 'w') as config:
        config.write(device_config)
        # build fs
    os.system('platformio run --target buildfs  --environment ' + target)
    # Upload firmware
    os.system('platformio run --target upload --upload-port /dev/ttyUSB0 --environment ' + target)
    # Upload fs
    os.system('platformio run --target uploadfs --upload-port /dev/ttyUSB0 --environment ' + target)
    sys.stdout.flush()


def main():
    #print('Attaching Debugger')
    # Allow other computers to attach to ptvsd at this IP address and port, using the secret
    #ptvsd.enable_attach("my_secret", address = ('0.0.0.0', 3000))
    # Pause the program until a remote debugger is attached
    #ptvsd.wait_for_attach()
    print('Starting MQTT client')
    client = paho.mqtt.client.Client(client_id='platformio', clean_session=False)
    #with open('/run/secrets/mqtt_username') as username, open('/run/secrets/mqtt_password') as password:
    #    client.username_pw_set(username.read().strip(), password.read().strip())
    client.username_pw_set(os.environ['MQTT_USERNAME'],os.environ['MQTT_PASSWORD'] )

    print('On_connect')
    client.on_connect = on_connect
    client.on_message = on_message
    print('TLS + Connect')
    with open('ca', 'w+') as ca:
        ca.write(os.environ['CA'])
    client.tls_set('ca', tls_version=ssl.PROTOCOL_TLSv1_2)
    client.connect(host='mqtt', port=8883)
    print('Finished setup, starting loop.')
    sys.stdout.flush()
    client.loop_forever()

if __name__ == '__main__':
    main()
    #sys.exit(0)