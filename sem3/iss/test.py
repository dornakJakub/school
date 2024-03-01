import numpy as np
import matplotlib.pyplot as plt
from matplotlib import gridspec


def ipy_coexp(k=2, params={}):
    """ 
    This function is used as paramater of interactive() in basis.ipynb notebook. 
    This implementation is in separate script, because very similar function is already presented in notebook. 
    """
    ck, i_zero, t, omega1 = params['ck'], params['i_zero'], params['t'], params['omega1']

    # coefficient c_0 - DC component
    x_synt = ck[i_zero] * np.ones(t.shape)

    # coefficients c_-k, ... c_-1, c_1, ..., c_k
    for kk in range(1, k+1):
        expo_pos = ck[i_zero + kk] * np.exp(+1j * kk * omega1 * t)
        expo_neg = ck[i_zero - kk] * np.exp(-1j * kk * omega1 * t)
        expo_sum = (expo_pos + expo_neg).real
        x_synt += expo_sum
    kk = k

    spec = gridspec.GridSpec(ncols=1, nrows=3, height_ratios=[3, 1, 1])
                         
    f = plt.figure(figsize=(16, 10))

    ax0 = f.add_subplot(spec[0], projection='3d')
    ax0.plot(t, np.real(expo_pos), np.imag(expo_pos), 'g', label=f'$c_{{ {kk}}}$')
    ax0.plot(t, np.real(expo_neg), np.imag(expo_neg), 'r', label=f'$c_{{ {-kk}}}$')
    ax0.legend()
    ax0.set_title(f'Komplexni exponenicaly odpovidajici $c_{{ {kk}}}$, $c_{{-{kk}}}$')
    
    ax0.grid()
    ax0.set_xticks([]), ax0.set_yticks([]), ax0.set_zticks([])
    ax0.set_xlabel('$t[s]$', labelpad=15)
    ax0.set_ylabel('Real', labelpad=15)
    ax0.set_zlabel('Imag', labelpad=15)

    ax1 = f.add_subplot(spec[1])
    ax1.plot(t, expo_sum, 'b')
    ax1.set_title(f'SouÄet dvou komplexnich exponencial odpovidajicim' \
                  + f' $c_{{ {kk}}}$, $c_{{-{kk}}}$')
    ax1.set_xlabel('$t[s]$')
    ax1.set_xlim(t.min(), t.max())
    if k >= 2:
        ax1.set_ylim(-2, 2)
    ax1.grid('--', alpha=0.6)
    
    ax2 = f.add_subplot(spec[2])
    ax2.plot(t, x_synt, 'b')
    ax2.set_title(f'Signal odpovidajici koeficientum $c_{{-{kk}, \dots , {kk}}}$')
    ax2.set_xlabel('$t[s]$')
    ax2.set_xlim(t.min(), t.max())
    ax2.grid('--', alpha=0.6)
    
    plt.subplots_adjust(right=1, left=0, top=0.9, bottom=0, hspace=0.5)  # plt.tight_layout()
