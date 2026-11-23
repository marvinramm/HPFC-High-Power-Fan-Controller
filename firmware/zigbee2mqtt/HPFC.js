// Zigbee2MQTT external converter for the HPFC (High Power Fan Controller)
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md
//
// HPFOS reports the model ID "HPFC_<chip ID>", so every board has its own ID.
// Replace the placeholder below with the model ID that Zigbee2MQTT logs when
// your board joins (Settings > Devices > About > "Zigbee Model").
// Several boards can be listed in zigbeeModel.

import * as m from 'zigbee-herdsman-converters/lib/modernExtend';

export default {
    zigbeeModel: ['HPFC_XXXXXXXXXXXX'],
    model: 'HPFC',
    vendor: 'Espressif',
    description: 'HPFC 4-channel fan controller with temperature, humidity and filter pressure',
    extend: [
        m.deviceEndpoints({endpoints: {'1': 1, '2': 2, '3': 3, '4': 4}}),

        // Endpoint 1: fan speed 0-100 %
        m.numeric({
            name: 'speed',
            valueMin: 0,
            valueMax: 100,
            valueStep: 1,
            cluster: 'genAnalogOutput',
            attribute: 'presentValue',
            reporting: {min: '0', max: '100', change: 1},
            description: 'Fan speed',
            unit: '%',
            access: 'ALL',
            endpointNames: ['1'],
        }),

        // Endpoint 2: air temperature (outlet sensor)
        m.numeric({
            name: 'temperature',
            cluster: 'genAnalogInput',
            attribute: 'presentValue',
            reporting: {min: '0', max: '100', change: 0.1},
            description: 'Air temperature',
            unit: '°C',
            access: 'STATE_GET',
            endpointNames: ['2'],
        }),

        // Endpoint 3: relative humidity (outlet sensor)
        m.numeric({
            name: 'humidity',
            cluster: 'genAnalogInput',
            attribute: 'presentValue',
            reporting: {min: '0', max: '100', change: 1},
            description: 'Relative humidity',
            unit: '%',
            access: 'STATE_GET',
            endpointNames: ['3'],
        }),

        // Endpoint 4: filter load = pressure difference in % of MAXPADELTA
        m.numeric({
            name: 'filter_load',
            cluster: 'genAnalogInput',
            attribute: 'presentValue',
            reporting: {min: '0', max: '100', change: 1},
            description: 'Filter load (pressure difference across the filter in % of the configured maximum)',
            unit: '%',
            access: 'STATE_GET',
            endpointNames: ['4'],
        }),
    ],
};
