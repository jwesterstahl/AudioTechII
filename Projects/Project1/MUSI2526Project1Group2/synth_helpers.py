import numpy as np
from scipy.io.wavfile import read, write
from scipy import signal
from scipy.signal import butter, filtfilt





# TODO: Replace the code below with your implementation of the waveforms.
# Hint: You may want to write more helper functions to create the waveforms
# Note: How will you handle aliasing?
def gen_wave(type, freq, dur, fs=44100, amp=1, phi=0):
    """ 
    Args:
    type (str) = waveform type: 'sine', 'square', 'saw', or 'triangle'
    freq (float) = fundamental frequency in Hz
    dur (float) = duration of the sinusoid (in seconds)
    fs (float) = sampling frequency of the sinusoid in Hz
    amp (float) = amplitude of the fundamental
    phi (float) = initial phase of the wave in radians
    Returns:
    The function should return a numpy array
    wave (numpy array) = The generated waveform
    """
    # Number of harmonics for creating fundamental waveforms
    NUM_HARMS = (((1/2)*44100) % freq)

    if dur <= 0:
        raise Exception("Cannot generate sinusoid with nonpositive duration.")
    if freq <= 0:
        raise Exception("Cannot generate sinusoid with nonpositive frequency.")
    if freq > fs / 2:
        raise Exception("WARNING: Frequency is over half of the provided sample rate, aliasing will occur.")
        

    n = np.arange(1, NUM_HARMS + 1)
    a = np.zeros_like(n)
    if type == 'sine':
        a[0] = 1
    elif type == 'saw':
        a = -1 / n * (-1 ** n)
    elif type == 'square':
        a = 1 / n * (n % 2)
    elif type == 'triangle':
        a = 1 / (n ** 2) * (n % 2) * np.sign(n % 4 - 2)
    else:
        raise Exception("Invalid wave type.")

    x = np.arange(0, dur, 1 / fs)

    # yes i researched numpy functions we haven't covered again for this i'm sorry
    # i really wanted to do this without a loop
    y = a[:, np.newaxis]*np.sin(2*np.pi*freq*np.outer(n, x) + phi)

    wave = amp * np.sum(y, axis=0)
    return wave
    

# Hint: If you use %'s for your ADSR lengths, what length should the sustain value be
# Note: How will you handle percentages that are too long? For example, attack is 50, decay is 50, release is 50?
def adsr(data, attack, decay, sustain, release, fs=44100):
    """
    Args:
    data (np.array) = signal to be modified
    attack (float) = value between 0-100 representing what percentage of the note duration the attack should be
    decay (float) = value between 0-100 representing what percentage of the note duration the attack should be
    sustain (float) = value between 0-1 representing the amplitude of the sustain
    release (float) = value between 0-100 representing what percentage of the note duration the attack should be
    fs (float) = sampling frequency of the sinusoid in Hz
    Returns:
    The function should return a numpy array
    sig (numpy array) = the modified, enveloped signal
    """
    a_len = int(data.size * attack / 100)
    d_len = int(data.size * decay / 100)
    r_len = int(data.size * release / 100)
    s_len = data.size - (a_len + d_len + r_len)

    a = np.linspace(0, 1, a_len)
    d = np.linspace(1, sustain, d_len)
    s = np.full(s_len, sustain)
    r = np.linspace(sustain, 0, r_len)

    env = np.concatenate((a, d, s, r))
    return data * env


# TODO: Replace the code below with your implementation of a FM synthesis
# Hint: You should really be doing PM.
def fm_synth(carrier_type, carrier_freq, mod_index, mod_ratio, dur, fs=44100, amp=1, modulator_type='sine'):
    """
    Args:
    carrier_type (str) = carrier waveform type: 'sine', 'square', 'saw', or 'triangle'
    carrier_freq (float) = frequency of carrier in Hz
    mod_index (float) = index of modulation
    mod_ratio (float) = modulation ratio, where modulator frequency = carrier_freq * mod_ratio
    dur (float) = duration of the sinusoid (in seconds)
    fs (float) = sampling frequency of the sinusoid in Hz
    amp (float) = amplitude of the carrier
    modulator_type (str) = modulator waveform type: 'sine', 'square', 'saw', or 'triangle'

    Returns:
    The function should return a numpy array
    sig (numpy array) = frequency modulated signal
    """

    fm = carrier_freq * mod_ratio
    mod = gen_wave(modulator_type, fm, dur, fs)
    sig = gen_wave(carrier_type, carrier_freq, dur, fs, amp, mod_index * mod)

    return sig

# TODO: Replace the code below with your implementation of a AM synthesis
def am_synth(carrier_type, carrier_freq, mod_depth, mod_ratio, dur, fs=44100, amp=1, modulator_type='sine'):
    """
    Args:
    carrier_type (str) = carrier waveform type: 'sine', 'square', 'saw', or 'triangle'
    carrier_freq (float) = frequency of carrier in Hz
    mod_depth (float) = depth of the modulator
    mod_ratio (float) = modulation ratio, where 1:mod_ratio is C:M
    dur (float) = duration of the sinusoid (in seconds)
    fs (float) = sampling frequency of the sinusoid in Hz
    amp (float) = amplitude of the carrier
    modulator_type (str) = modulator waveform type: 'sine', 'square', 'saw', or 'triangle'

    Returns:
    The function should return a numpy array
    sig (numpy array) = amplitude modulated signal
    """

    fm = carrier_freq * mod_ratio
    if fm > (fs/2):
        raise Exception(f'Modulation harmonics cannot exceed {fs/2}')
    mod = (gen_wave(modulator_type, fm, dur, fs, mod_depth) + 1) / 2
    sig = gen_wave(carrier_type, carrier_freq, dur, fs)

    return amp * sig * mod


# TODO: Complete at least one of the functions below: filter, reverb, delay.

# Note: I wrote this to only create low or highpass filters. You can alter to create bandpass/bandstop, but do not change the function definition.
def filter(data, type, cutoff_freq, fs=44100, order=5):
    """
    Args:
    data (np.array) = signal to be modified
    type (str) = filter type 'lowpass' or 'highpass'
    cutoff_freq (float) = cutoff frequency in Hz
    fs (float) = sampling frequency of the sinusoid in Hz
    order (int) = filter order

    Returns:
    The function should return a numpy array
    sig (numpy array) = filtered signal
    """
    (b, a) = butter(order, cutoff_freq, btype=type, fs=fs)

    sig = filtfilt(b, a, data)
    
    return sig

def reverb(data, ir, dry_wet=0.5):
    """
    Args:
    data (np.array) = signal to be modified
    ir (str) = file path to impulse response
    dry_wet (float) = value between 0-1 dry/wet balance

    Returns:
    The function should return a numpy array
    sig (numpy array) = signal with reverb
    """

    (fs, ir) = read(ir)
    ir = ir[:,0]

    if ir.dtype == np.int16:
        ir = ir.astype(np.float32) / 32768.0
    else:
        ir = ir.astype(np.float32)
        
    wet = np.convolve(data,ir, mode='same')
    sig = (1 - dry_wet) * data + dry_wet * wet

    return sig

def delay(data, delay_time, dry_wet=0.5, fs=44100):
    """
    Args:
    data (np.array) = signal to be modified
    delay_time (float) = delay time in seconds
    dry_wet (float) = value between 0-1 dry/wet balance
    fs (float) = sampling frequency of the sinusoid in Hz

    Returns:
    The function should return a numpy array
    sig (numpy array) = signal with a delay
    """

    sig = data
    copy = data.copy()
    m = delay_time
    offset = (m / 1000) * fs
    pad = np.zeros(int(offset))
    orig = np.concatenate([data,pad])
    delay = np.concatenate([pad,copy])
    sig = orig+(delay * dry_wet)

    return sig

