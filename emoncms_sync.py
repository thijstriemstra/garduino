# Copyright 2021-2023 Collab

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
    # logging
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s %(levelname)-5s - %(message)s'
    )
    # silence urllib3
    urllib3_logger = logging.getLogger('urllib3')
    urllib3_logger.setLevel(logging.CRITICAL)

    desc = 'Post local emonpi data to emoncms.org'

    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter, description=desc)
    parser.add_argument('--emonpi', dest='local', default='http://127.0.0.1', type=str, help='Address of local emonpi')
    parser.add_argument('--emoncms', dest='remote', default='https://emoncms.org', type=str, help='Address of remote emoncms')
    parser.add_argument('--api-key-write', required=True, dest='writeKey', type=str, help='Write API key')
    parser.add_argument('--api-key-read', required=True, dest='readKey', type=str, help='Read API key')

    args = parser.parse_args()

    logger.info(desc)
    logger.info(f"Local:\t{args.local}")
    logger.info(f"Remote:\t{args.remote}")
    logger.info("")

    local_url = f"{args.local}/feed/value.json"
    remote_url = f"{args.remote}/feed/insert.json"

    # sync data
    result = []
    for feed in getKeys():
        feed_id = feed.get('local_feed')

        # read
        payload = {'id': feed_id, 'apikey': args.readKey}
        r = requests.get(local_url, params=payload)

        try:
            val = r.json()
            result.append(val)

            # timestamp
            dt = datetime.now()
            timestamp = dt.replace(tzinfo=timezone.utc).timestamp()

            # payload
            payload = {
                'id': feed.get('remote_feed'),
                'apikey': args.writeKey,
                'time': timestamp,
                'value': val
            }

            # send it
            p = requests.post(remote_url, params=payload)
            logger.info("local: {} - remote: {} - value: {} - response: {}".format(
                feed.get("local_feed"), feed.get("remote_feed"), val, p.status_code))

        except json.decoder.JSONDecodeError as e:
            logger.error("Something went wrong with feed id {}".format(feed_id))
            raise e


if __name__ == "__main__":
    main()
