# Copyright 2021 Collab

"""
Post local emonpi data to emoncms.org
"""

import json
import logging
import argparse
import requests
from datetime import timezone, datetime

logger = logging.getLogger(__name__)


def getKeys():
    return [
        {'local_feed': 1, 'remote_feed': 413104},
        {'local_feed': 2, 'remote_feed': 413098},
        {'local_feed': 3, 'remote_feed': 413097},
        {'local_feed': 4, 'remote_feed': 413099},
        {'local_feed': 5, 'remote_feed': 413095},
        {'local_feed': 6, 'remote_feed': 413954},
        {'local_feed': 7, 'remote_feed': 413953},
        {'local_feed': 8, 'remote_feed': 413096},
        {'local_feed': 9, 'remote_feed': 413103}
    ]


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description='Post data to emoncms.org'
    )
    parser.add_argument('--emonpi', dest='local', default='http://127.0.0.1', type=str, help='Address of local emonpi')
    parser.add_argument('--emoncms', dest='remote', default='https://emoncms.org', type=str, help='Address of remote emoncms')
    parser.add_argument('--api-key-write', required=True, dest='writeKey', type=str, help='Write API key')
    parser.add_argument('--api-key-read', required=True, dest='readKey', type=str, help='Read API key')

    args = parser.parse_args()

    print()
    print("Local:\t{}".format(args.local))
    print("Remote:\t{}".format(args.remote))
    print()

    local_url = "{}/feed/value.json".format(args.local)
    remote_url = "{}/feed/insert.json".format(args.remote)

    # sync data
    result = []
    for feed in getKeys():
        # read
        payload = {'id': feed.get('local_feed'), 'apikey': args.readKey}
        r = requests.get(local_url, params=payload)

        try:
            val = r.json()
            result.append(val)

            dt = datetime.now()
            timestamp = dt.replace(tzinfo=timezone.utc).timestamp()

            # send
            payload = {
                'id': feed.get('remote_feed'),
                'apikey': args.writeKey,
                'time': timestamp,
                'value': val
            }
            p = requests.post(remote_url, params=payload)
            print("local: {} - remote: {} - value: {} - response: {}".format(
                feed.get("local_feed"), feed.get("remote_feed"), val, p.status_code))

        except json.decoder.JSONDecodeError as e:
            print("Something went wrong with feed id {}".format(feed_id))
            raise e


if __name__ == "__main__":
    main()
