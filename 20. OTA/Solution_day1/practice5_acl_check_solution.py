import getpass
import os

import paho.mqtt.client as mqtt


WRITE_PERMISSIONS = {'write', 'readwrite'}
ALL_PERMISSIONS = WRITE_PERMISSIONS | {'read', 'deny'}


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        print('Connected OK')
    else:
        print('Connection failed, reason code:', reason_code)


def on_disconnect(client, userdata, rc):
    print('Disconnected, RC:', rc)


def on_publish(client, userdata, mid):
    print('Published message, MID:', mid)


def prompt_input(prompt_text, default=None):
    suffix = f' [{default}]: ' if default is not None else ': '
    while True:
        value = input(prompt_text + suffix).strip()
        if value:
            return value
        if default is not None:
            return default
        print('입력이 필요합니다.')


def parse_acl_file(acl_path):
    if not os.path.isfile(acl_path):
        raise FileNotFoundError(f'ACL 파일을 찾을 수 없습니다: {acl_path}')

    user_rules = {}
    current_user = None

    with open(acl_path, 'r', encoding='utf-8') as acl_file:
        for line_number, raw_line in enumerate(acl_file, start=1):
            line = raw_line.strip()
            if not line or line.startswith('#'):
                continue

            parts = line.split()
            directive = parts[0].lower()

            if directive == 'user':
                if len(parts) != 2:
                    raise ValueError(f'{line_number}행: user 구문이 올바르지 않습니다.')
                current_user = parts[1]
                user_rules.setdefault(current_user, [])
                continue

            if directive not in ('topic', 'pattern') or current_user is None:
                continue

            if len(parts) < 2:
                raise ValueError(f'{line_number}행: topic/pattern 구문이 올바르지 않습니다.')

            if parts[1].lower() in ALL_PERMISSIONS:
                permission = parts[1].lower()
                topic_filter = ' '.join(parts[2:])
            else:
                permission = 'readwrite'
                topic_filter = ' '.join(parts[1:])

            if not topic_filter:
                raise ValueError(f'{line_number}행: topic 필터가 없습니다.')

            user_rules[current_user].append({
                'kind': directive,
                'permission': permission,
                'filter': topic_filter,
            })

    return user_rules


def topic_matches(topic_filter, topic):
    filter_levels = topic_filter.split('/')
    topic_levels = topic.split('/')

    for index, filter_level in enumerate(filter_levels):
        if filter_level == '#':
            return index == len(filter_levels) - 1
        if index >= len(topic_levels):
            return False
        if filter_level != '+' and filter_level != topic_levels[index]:
            return False

    return len(filter_levels) == len(topic_levels)


def can_publish(rules, username, topic):
    matched_allow = False

    for rule in rules:
        topic_filter = rule['filter'].replace('%u', username)
        if '%c' in topic_filter or not topic_matches(topic_filter, topic):
            continue
        if rule['permission'] == 'deny':
            return False
        if rule['permission'] in WRITE_PERMISSIONS:
            matched_allow = True

    return matched_allow


def publish_message(broker_ip, port, username, password, topic, message):
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish
    client.username_pw_set(username, password)

    client.connect(broker_ip, port)
    client.loop_start()
    publish_info = client.publish(topic, message, qos=1)
    publish_info.wait_for_publish()
    client.loop_stop()
    client.disconnect()


def main():
    print('=== Practice 5 ACL Topic Access Check ===')
    acl_path = prompt_input('ACL 파일 경로', 'acl_file')
    username = prompt_input('MQTT 사용자명')

    try:
        rules = parse_acl_file(acl_path).get(username, [])
    except Exception as exc:
        print('ACL 파싱 오류:', exc)
        return

    if not rules:
        print(f"사용자 '{username}'에 대한 ACL 항목을 찾을 수 없습니다.")
        return

    print(f"사용자 '{username}'의 ACL 규칙:")
    for rule in rules:
        print(f" - {rule['kind']} {rule['permission']} {rule['filter']}")

    topic = prompt_input('Publish 권한을 확인할 Topic')
    if not can_publish(rules, username, topic):
        print(f"ACL 기준으로 '{topic}'에 publish할 수 없습니다.")
        return

    print(f"ACL 기준으로 '{topic}'에 publish할 수 있습니다.")
    broker_ip = prompt_input('Broker IP', 'localhost')
    port_text = prompt_input('Broker Port', '1883')
    message = prompt_input('Message', 'Hello! ACL test message.')
    password = getpass.getpass('MQTT password: ')

    try:
        port = int(port_text)
        publish_message(broker_ip, port, username, password, topic, message)
    except ValueError:
        print('Broker Port는 숫자여야 합니다.')
    except Exception as exc:
        print('Publish error:', exc)


if __name__ == '__main__':
    main()
