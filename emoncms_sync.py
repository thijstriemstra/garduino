# Copyright 2021 Collab

"""
Post local emonpi data to emoncms.org
"""

import json
import argparse
import requests


def getKeys():
    return [x for x in range(1, 8)]


def main():
    parser = argparse.ArgumentParser(description='Post data to emoncms.org')
    parser.add_argument('--emonpi', dest='local', default='http://127.0.0.1', type=str, help='Address of local emonpi')
    parser.add_argument('--emoncms', dest='remote', default='https://emoncms.org', type=str, help='Address of remote emoncms')
    parser.add_argument('--api-key-write', required=True, dest='writeKey', type=str, help='Write API key')
    parser.add_argument('--api-key-read', required=True, dest='readKey', type=str, help='Read API key')
    parser.add_argument('--node', dest='node', type=int, default=4, help='Remote node id')

    args = parser.parse_args()

    print()
    print("Local:\t{}".format(args.local))
    print("Remote:\t{}".format(args.remote))
    print()

    local_url = "{}/emoncms/feed/value.json".format(args.local)
    remote_url = "{}/input/post".format(args.remote)

    # sync data
    for feed_id in getKeys():
        # read
        payload = {'id': feed_id, 'apikey': args.readKey}
        r = requests.get(local_url, params=payload)

        try:
            print(r.json())

            # send
            payload = {'node': args.node, 'apikey': args.writeKey, 'fulljson': []}
            p = requests.post(remote_url, params=payload)
            print(r.json())

        except json.decoder.JSONDecodeError as e:
            print("Something went wrong with feed id {}".format(feed_id))
            raise e


if __name__ == "__main__":
    main()
