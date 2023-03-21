# ESPHome

Checkout the code:

```console
git clone https://github.com/thijstriemstra/garduino.git
cd garduino
```

Create a [virtual environment](https://virtualenvwrapper.readthedocs.io/en/latest/index.html)
called `garduino`:

```console
mkvirtualenv garduino
```

Install the dependencies:

```console
pip install -e .
```

Verify installation:

```console
esphome version
```

Create `secrets.yaml`:

```console
touch secrets.yaml
```

And make sure to add all options marked with `!secret` in `garduino.yaml`.

### Upload

Plug in the device via USB and type the following command:

```console
esphome run garduino.yaml
```

You should see ESPHome validating the configuration and telling you about
potential problems. Then ESPHome will proceed to compile and upload the
custom firmware.
