#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from setuptools import setup


setup(
    name="py.garden",
    version='0.0.1',
    author="Thijs Triemstra",
    author_email="info@collab.nl",
    description="Python scripts for garden monitoring using ESP32",
    license="MIT",
    keywords="esp32 micropython DS18B20 BH1750 raindrops soil detection rain sensor temperature moisture",
    url="https://github.com/thijstriemstra/py-garden",
    packages=["pygarden"],
    install_requires=[
        'pyserial',
        'adafruit-ampy'
    ],
    extras_require={
        'docs': [
            'sphinx >= 1.5.1'
        ],
        'qa': [
            'flake8'
        ]
    },
    zip_safe=False,
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Education",
        "Intended Audience :: Developers",
        "Topic :: Education",
        "Topic :: System :: Hardware",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6"
    ]
)
