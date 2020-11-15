# Array of tests to run (in order)
# Each test contains
#   description -
#   steps - A list of steps to perform, each step can have
#       inputs - A list of tuples for the inputs to apply at that step
#       *time - The time (in ms) to wait before continuing to the next step
#           and before checking expected values for this step. The time should be a multiple of
#           the period of the system
#       *iterations - The number of clock ticks to wait (periods)
#       expected - The expected value at the end of this step (after the "time" has elapsed.)
#           If this value is incorrect the test will fail early before completing.
#       * only one of these should be used
#   expected - The expected output (as a list of tuples) at the end of this test
# An example set of tests is shown below. It is important to note that these tests are not "unit tests" in
# that they are not ran in isolation but in the order shown and the state of the device is not reset or
# altered in between executions (unless preconditions are used).
tests = [
    {'description': 'Check initialization',
        'steps': [ {'inputs': [('PINA',0x00)], 'iterations': 1 } ],
        'expected': [('PORTC', 0x07)],
    },
    {'description': 'Check single increment',
        'steps': [ {'inputs': [('PINA',0x01) ], 'iterations': 13 },
                {'inputs': [('PINA',0x01) ], 'iterations': 2 }],
        'expected': [('PORTC', 0x08),],
        #'expected': [('PORTC', 0x08),],
    },
    {'description': 'Check single decrement',
        'steps': [ {'inputs': [('PINA',0x00)], 'iterations': 2 },
                {'inputs': [('PINA',0x02)], 'iterations': 3 }],
        'expected': [('PORTC', 0x07)],
    },
    {'description': 'Check increment at 9',
        'steps': [ {'inputs': [('PINA',0x00)], 'iterations': 2 },
                {'inputs': [('PINA',0x01)], 'iterations': 3 },
                {'inputs': [('PINA',0x00)], 'iterations': 3 },
                {'inputs': [('PINA',0x01)], 'iterations': 3 },
                {'inputs': [('PINA',0x00)], 'iterations': 3 },
                {'inputs': [('PINA',0x01)], 'iterations': 3 }],
        'expected': [('PORTC', 0x09)],
    },
    {'description': 'Check reset',
        'steps': [ {'inputs': [('PINA',0x00)], 'iterations': 2 },
                {'inputs': [('PINA',0x03)], 'iterations': 3 },
                {'inputs': [('PINA',0x02)], 'iterations': 1 },
                {'inputs': [('PINA',0x03)], 'iterations': 3 },
                {'inputs': [('PINA',0x01)], 'iterations': 1 },
                {'inputs': [('PINA',0x03)], 'iterations': 3 }],
        'expected': [('PORTC', 0x00)],
    },
    {'description': 'Check decrement at 0',
        'steps': [ {'inputs': [('PINA',0x00)], 'iterations': 2 },
                {'inputs': [('PINA',0x02)], 'iterations': 3 },
                {'inputs': [('PINA',0x00)], 'iterations': 3 },
                {'inputs': [('PINA',0x02)], 'iterations': 3 },
                {'inputs': [('PINA',0x00)], 'iterations': 3 },
                {'inputs': [('PINA',0x02)], 'iterations': 3 }],
        'expected': [('PORTC', 0x00)],
    },
    ]

# Optionally you can add a set of "watch" variables these need to be global or static and may need
# to be scoped at the function level (for static variables) if there are naming conflicts. The
# variables listed here will display everytime you hit (and stop at) a breakpoint
watch = ['counter::tmpC', 'main::state', 'PINA', 'PORTC']
