"""
Copyright (C) 2022 - Ruchita Korgaonkar <ruchita@iitb.ac.in>,
Mahesh Patil <mbpatil@ee.iitb.ac.in>
This file is part of GSEIM.

GSEIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

import sys
import time
import numpy as np
import csv
import matplotlib
import matplotlib.pylab as plt
from matplotlib.ticker import MaxNLocator

from os.path import expanduser
from PyQt6.QtCore import *
from PyQt6.QtGui import *
from PyQt6.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QRadioButton,
    QFrame, QMenu, QScrollArea, QLineEdit, QFileDialog,
    QComboBox, QMainWindow, QSizePolicy, QVBoxLayout, QListWidget,
    QCheckBox, QHeaderView, QListWidgetItem, QAbstractItemView,
    QHBoxLayout, QColorDialog, QPlainTextEdit, QMessageBox,
    QTableWidget, QTableWidgetItem
    )
import numpy as np
import random
import os.path
from os import path
from matplotlib.backends.qt_compat import QtCore, QtWidgets
from matplotlib.figure import Figure

print('QT6')
from matplotlib.backends.backend_qt5agg import (
    FigureCanvas, NavigationToolbar2QT as NavigationToolbar)

class GSEIMPlotNavigationToolbar(NavigationToolbar):
    toolitems = [t for t in NavigationToolbar.toolitems if
                 t[0] in ('Home', 'Pan', 'Zoom', 'Save')]

def fourier_coeff(t, x, t_start, t_end, n_fourier):

     coeff = [0.0]*n_fourier
     sum_fourier_real = [0.0]*n_fourier
     sum_fourier_imag = [0.0]*n_fourier
     sum_fourier_mag = [0.0]*n_fourier
     omg = [0.0]*n_fourier
     sum_rms_1 = 0.0

     T = t_end - t_start
     a2 = 0.5*T
     f_hz = 1.0/T
     for i in range(n_fourier):
         omg[i] = 2.0*float(i)*np.pi/T

     t_last = t[0];
     y_last = x[0];

     n = len(t)

     for i in range(n):
         t0 = t[i]
         y0 = x[i]

         if t_last <= t_end:
             if t_last >= t_start:
                 if t0 <= t_end:
                     delta_t = t0 - t_last
                     for j in range(n_fourier):
                         wt1 = omg[j]*t_last
                         wt2 = omg[j]*t0
                         z1 = y_last
                         z2 = y0
                         sum_fourier_real[j] += 0.5*delta_t*(np.cos(wt1)*z1 + np.cos(wt2)*z2)
                         sum_fourier_imag[j] += 0.5*delta_t*(np.sin(wt1)*z1 + np.sin(wt2)*z2)
                     sum_rms_1 += 0.5*delta_t*(y_last*y_last + y0*y0)
                 else:
                     y1 = y_last + ((y0-y_last)/(t0-t_last))*(t_end-t_last)
                     delta_t = t_end-t_last
                     for j in range(n_fourier):
                         wt1 = omg[j]*t_last
                         wt2 = omg[j]*t_end
                         z1 = y_last
                         z2 = y1
                         sum_fourier_real[j] += 0.5*delta_t*(np.cos(wt1)*z1 + np.cos(wt2)*z2)
                         sum_fourier_imag[j] += 0.5*delta_t*(np.sin(wt1)*z1 + np.sin(wt2)*z2)
                     sum_rms_1 += 0.5*delta_t*(y_last*y_last + y1*y1)
             else:
                 if t0 > t_start:
                     y1 = y_last + ((y0-y_last)/(t0-t_last))*(t_start-t_last)
                     delta_t = t0-t_start
                     for j in range(n_fourier):
                         wt1 = omg[j]*t_start
                         wt2 = omg[j]*t0
                         z1 = y1
                         z2 = y0
                         sum_fourier_real[j] += 0.5*delta_t*(np.cos(wt1)*z1 + np.cos(wt2)*z2)
                         sum_fourier_imag[j] += 0.5*delta_t*(np.sin(wt1)*z1 + np.sin(wt2)*z2)
                     sum_rms_1 += 0.5*delta_t*(y1*y1 + y0*y0)
         t_last = t0
         y_last = y0

     for i in range(n_fourier):
         b1 = sum_fourier_real[i]
         b2 = sum_fourier_imag[i]
         sum_fourier_mag[i] = np.sqrt((b1*b1)+(b2*b2))/a2

     coeff[0] = 0.5*sum_fourier_mag[0]
     for i in range(1, n_fourier):
         coeff[i] = sum_fourier_mag[i]

     arg1 = 2.0*(sum_rms_1/T) - 2.0*coeff[0]**2 - coeff[1]**2
     thd = np.sqrt(arg1)/coeff[1]

     return coeff, thd

class ScriptObject(QWidget):
    def __init__(self):
        super().__init__()
        self.title = "generatePlot.py"
        self.InitWindow()

    def InitWindow(self):
        self.setWindowTitle(self.title)
        vbox = QVBoxLayout()
        self.frame16 = QFrame(self)
        self.frame16.setGeometry(QRect(1,1,100,25))
        self.saveBtn = QPushButton("Save Script",self.frame16)
        vbox.addWidget(self.saveBtn)
        self.saveBtn.clicked.connect(self.openFileSaveDialog)
        self.saveBtn.setEnabled(1)

        self.frame1 = QFrame(self);self.frame1.setGeometry(QRect(1, 25, 600, 300))
        self.plainText = QPlainTextEdit(self.frame1)
        self.plainText.resize(600,300)
        self.plainText.setPlaceholderText("dummy text")

        self.plainText.setReadOnly(False)

        print('InitWindow: multiPlot:', mainWin.multiPlotObject_1.multiPlot)

        if mainWin.powerObject_1.compute_power:
            n_plots = 3
            self.text = "import matplotlib.pyplot as plt"
            self.text += "\nimport numpy as np"
            self.text += "\nfrom matplotlib.ticker import MaxNLocator"

            self.text += "\n"
            self.text += "\nreader_power = np.loadtxt('" + mainWin.filename_power + "')"
            self.text += "\nt_power = reader_power[:,0]"
            self.text += "\nv_rms   = reader_power[:,1]"
            self.text += "\ni_rms   = reader_power[:,2]"
            self.text += "\np_avg   = reader_power[:,3]"
            self.text += "\np_app   = reader_power[:,4]"
            self.text += "\npf      = reader_power[:,5]"

            self.text += "\n"
            self.text += "\nreader = np.loadtxt('" + mainWin.fileName + "')"
            self.text += "\nt = reader[:, 0]"
            self.text += "\nvoltage = reader[:, " + str(mainWin.v_index) + "]"
            self.text += "\ncurrent = reader[:, " + str(mainWin.i_index) + "]"

            self.text += "\n"
            self.text += "\nfig, axs = plt.subplots(" + str(n_plots) + \
               ", 1, sharex=True)"
            self.text += "\nfig.subplots_adjust(hspace=0.0)"
            self.text += "\n"
            self.text += "\nfor ax in axs:"
            self.text += "\n    ax.label_outer"
            self.text += "\n    ax.grid(color='lightgrey')"
            self.text += "\n    ax.legend(loc='best')"
            self.text += "\n    ax.ticklabel_format(axis='x', style='sci'," + \
                " scilimits=(-2,2), useMathText=True)"
            self.text += "\n    ax.ticklabel_format(axis='y', style='sci'," + \
                " scilimits=(-2,2), useMathText=True)"

            self.text += "\n"
            self.text += "\npl1 = axs[0].plot(t, voltage," + \
                 " color='dodgerblue'," + \
                 " linestyle='solid'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='" + mainWin.v_string + "')"
            self.text += "\naxs[0].plot(t_power, v_rms," + \
                 " color='dodgerblue'," + \
                 " linestyle='dashed'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='')"

            self.text += "\n"
            self.text += "\nax2_1 = axs[0].twinx()"
            self.text += "\npl2 = ax2_1.plot(t, current," + \
                 " color='orangered'," + \
                 " linestyle='solid'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='" + mainWin.i_string + "')"
            self.text += "\nax2_1.plot(t_power, i_rms," + \
                 " color='orangered'," + \
                 " linestyle='dashed'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='')"

            self.text += "\n"
            self.text += "\npls = pl1 + pl2"
            self.text += "\nlabs = [l.get_label() for l in pls]"
            self.text += "\naxs[0].legend(pls, labs, loc='best')"

            self.text += "\n"
            self.text += "\naxs[1].plot(t_power, p_avg," + \
                 " color='teal'," + \
                 " linestyle='solid'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='p_avg')"
            self.text += "\naxs[1].plot(t_power, p_app," + \
                 " color='deeppink'," + \
                 " linestyle='solid'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='p_app')"
            self.text += "\naxs[1].legend()"

            self.text += "\n"
            self.text += "\naxs[2].plot(t_power, pf," + \
                 " color='darkgoldenrod'," + \
                 " linestyle='solid'," + \
                 " linewidth=1.0," + \
                 "\n  " + \
                 " drawstyle='default'," + \
                 " label='pf')"
            self.text += "\naxs[2].legend()"

            if (mainWin.axesPropObject_1.s_xMin.split()) or (mainWin.axesPropObject_1.s_xMax.split()):
                self.text += "\nfor i in range(3):"
                if (mainWin.axesPropObject_1.s_xMin.split()):
                    self.text += "\n    axs[i].set_xlim(left =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMin)) + ")"
                if (mainWin.axesPropObject_1.s_xMax.split()):
                    self.text += "\n    axs[i].set_xlim(right =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMax)) + ")"

            self.text += "\n"
            self.text += "\nplt.show()"
            self.text += "\n"
        elif mainWin.fourierObject_1.fourier:
            mainWin.n_variables = len(mainWin.YIndex) + len(mainWin.YIndexR)
            self.text = "import matplotlib.pyplot as plt"
            self.text += "\nimport numpy as np"
            self.text += "\nreader = np.loadtxt('" + mainWin.filename_fourier + "')"
            self.text += "\nx = reader[:,0]"
            self.text += "\nfig, axs = plt.subplots(" + str(mainWin.n_variables) + \
               ", 1, sharex=True)"

            if mainWin.n_variables > 1:
                self.text += "\nfig.subplots_adjust(hspace=0.0)"
                self.text += "\nfor ax in axs:"
                self.text += "\n    ax.label_outer"
                self.text += "\n    ax.grid(color='lightgrey')"
                self.text += "\n    ax.legend(loc='best')"
                self.text += "\n    ax.set_axisbelow(True)"
                self.text += "\n    ax.xaxis.set_major_locator(MaxNLocator(integer=True))"
                self.text += "\n    ax.ticklabel_format(axis='y', style='sci'," + \
                    " scilimits=(-2,2), useMathText=True)"
            else:
                self.text += "\naxs.label_outer"
                self.text += "\naxs.grid(color='lightgrey')"
                self.text += "\naxs.legend(loc='best')"
                self.text += "\naxs.set_axisbelow(True)"
                self.text += "\naxs.xaxis.set_major_locator(MaxNLocator(integer=True))"
                self.text += "\naxs.ticklabel_format(axis='y', style='sci'," + \
                    " scilimits=(-2,2), useMathText=True)"

            if mainWin.n_variables == 1:
                y_thd = 0.88
            elif mainWin.n_variables == 2:
                y_thd = 0.78
            elif mainWin.n_variables == 3:
                y_thd = 0.68

            self.text += "\nthd = np.loadtxt('" + mainWin.filename_thd + "')"
            bar_width = 0.5

            if mainWin.n_variables > 1:
                i_ax = 0
                for i in range(0,len(mainWin.YIndex)):
                    self.text += "\n\ny = reader[:, " + str(i_ax+1) + "]"

                    self.text += "\naxs[" + str(i_ax) + "].bar(x, y," + \
                                 "\n   width=" + str(bar_width) + ", color='darkorange', align='center'," + \
                                 "\n   label='" + mainWin.plotObject_1[mainWin.YIndex[i]].label + "')"

                    self.text += "\naxs[" + str(i_ax) + "].legend()"
                    self.text += "\naxs[" + str(i_ax) + "].set_axisbelow(True)"
    
                    self.text += "\nthd1 = thd[" + str(i_ax) + "]"

                    self.text += "\ns = 'THD: '"
                    self.text += "\ns += '%6.2f %%'%(100.0*thd1)"
                    self.text += "\naxs[" + str(i_ax) + "].text(0.9, " + '%4.2f'%(y_thd) + ", s,"
                    self.text += "\n   horizontalalignment='center',"
                    self.text += "\n   verticalalignment='top',"
                    self.text += "\n   transform=axs[" + str(i_ax) + "].transAxes)"

                    i_ax += 1
                for i in range(0,len(mainWin.YIndexR)):
                    self.text += "\n\ny = reader[:, " + str(i_ax+1) + "]"
                    self.text += "\naxs[" + str(i_ax) + "].bar(x, y," + \
                                 "\n   width=" + str(bar_width) + ", color='darkorange', align='center'," + \
                                 "\n   label='" + mainWin.plotObject_1[mainWin.YIndexR[i]].label + "')"
                    self.text += "\naxs[" + str(i_ax) + "].legend()"
                    self.text += "\naxs[" + str(i_ax) + "].set_axisbelow(True)"

                    self.text += "\nthd1 = thd[" + str(i_ax) + "]"

                    self.text += "\ns = 'THD: '"
                    self.text += "\ns += '%6.2f %%'%(100.0*thd1)"
                    self.text += "\naxs[" + str(i_ax) + "].text(0.9, " + '%4.2f'%(y_thd) + ", s,"
                    self.text += "\n   horizontalalignment='center',"
                    self.text += "\n   verticalalignment='top',"
                    self.text += "\n   transform=axs[" + str(i_ax) + "].transAxes)"

                    i_ax += 1
            else:
                self.text += "\n\ny = reader[:, 1]"
                for i in range(0,len(mainWin.YIndex)):
                    self.text += "\naxs.bar(x, y," + \
                                 "\n   width=" + str(bar_width) + ", color='darkorange', align='center'," + \
                                 "\n   label='" + mainWin.plotObject_1[mainWin.YIndex[i]].label + "')"
                for i in range(0,len(mainWin.YIndexR)):
                    self.text += "\naxs.bar(x, y," + \
                                 "\n   width=" + str(bar_width) + ", color='darkorange', align='center'," + \
                                 "\n   label='" + mainWin.plotObject_1[mainWin.YIndexR[i]].label + "')"
                self.text += "\naxs.legend()"
                self.text += "\naxs.set_axisbelow(True)"
    
                self.text += "\nthd1 = thd.tolist()"

                self.text += "\ns = 'THD: '"
                self.text += "\ns += '%6.2f %%'%(100.0*thd1)"
                self.text += "\naxs.text(0.9, " + '%4.2f'%(y_thd) + ", s,"
                self.text += "\n   horizontalalignment='center',"
                self.text += "\n   verticalalignment='top',"
                self.text += "\n   transform=axs.transAxes)"

            self.text += "\n"
            self.text += "\nplt.show()"
            self.text += "\n"
        elif mainWin.multiPlotObject_1.multiPlot:
            if mainWin.n_variables == 1:
                mainWin.displayMessage1("Multi-plot error", "Must choose more than one variables.")
                sys.exit()

            if not (mainWin.avgrmsObject_1.avg or mainWin.avgrmsObject_1.rms):
                self.text = "import matplotlib.pyplot as plt"
                self.text += "\nimport numpy as np"
                self.text += "\nreader = np.loadtxt('" + mainWin.fileName + "')"
                self.text += "\nfig, axs = plt.subplots(" + str(mainWin.n_variables) + \
                   ", 1, sharex=True)"
                self.text += "\nfig.subplots_adjust(hspace=0.0)"
                self.text += "\nfor ax in axs:"
                self.text += "\n    ax.label_outer"
                self.text += "\n    ax.grid(color='lightgrey')"
                self.text += "\n    ax.ticklabel_format(axis='x', style='sci'," + \
                    " scilimits=(-2,2), useMathText=True)"

                self.text += "\nx = reader[:, " + str(mainWin.XIndex) + "]"
                i_ax = 0
                for i in range(0,len(mainWin.YIndex)):
                    if (mainWin.plotObject_1[mainWin.YIndex[i]].multiLinLog == 'log'):
                        self.text += "\n\ny = np.log10(reader[:, " + mainWin.YIndex[i] + "])"
                    else:
                        self.text += "\n\ny = reader[:, " + str(mainWin.YIndex[i]) + "]"

                    if mainWin.plotObject_1[mainWin.YIndex[i]].lineStyle:
                        lineStyle = mainWin.plotObject_1[mainWin.YIndex[i]].lineStyle
                    else:
                        lineStyle = 'solid'

                    if mainWin.plotObject_1[mainWin.YIndex[i]].lineColor:
                        lineColor = mainWin.plotObject_1[mainWin.YIndex[i]].lineColor
                    else:
                        pltColor = mainWin.colorSet[(mainWin.YIndex[i])%mainWin.nColorSet]
                        lineColor = pltColor

                    lineWidth = mainWin.plotObject_1[mainWin.YIndex[i]].width
                    drawStyle = mainWin.plotObject_1[mainWin.YIndex[i]].drawStyle
                    label = mainWin.plotObject_1[mainWin.YIndex[i]].label
                    marker = mainWin.plotObject_1[mainWin.YIndex[i]].marker
                    markerSize = mainWin.plotObject_1[mainWin.YIndex[i]].size
                    markerEdgeColor = mainWin.plotObject_1[mainWin.YIndex[i]].edgeColor
                    markerFaceColor = mainWin.plotObject_1[mainWin.YIndex[i]].faceColor

                    self.text += "\naxs[" + str(i_ax) + "].plot(x, y," + \
                         " color='" + str(lineColor) + "'," + \
                         " linestyle='" + str(lineStyle) + "'," + \
                         " linewidth=" + str(lineWidth) + "," + \
                         "\n  " + \
                         " drawstyle='" + str(drawStyle) + "'," + \
                         " label='" + str(label) + "'," + \
                         " marker='" + str(marker) + "'," + \
                         "\n  " + \
                         " markersize=" + str(markerSize) + "," + \
                         " markeredgecolor='" + str(markerEdgeColor) + "'," + \
                         " markerfacecolor='" + str(markerFaceColor) + "')"

                    if mainWin.plotObject_1[mainWin.YIndex[i]].multiLinLog == 'linear':
                        self.text += "\naxs[" + str(i_ax) + "].ticklabel_format(axis='y', style='sci'," + \
                            " scilimits=(-2, 2),useMathText=True)"
                    i_ax += 1
                for i in range(0,len(mainWin.YIndexR)):
                    if (mainWin.plotObject_1[mainWin.YIndexR[i]].multiLinLog == 'log'):
                        self.text += "\n\ny = np.log10(reader[:, " + mainWin.YIndexR[i] + "])"
                    else:
                        self.text += "\n\ny = reader[:, " + str(mainWin.YIndexR[i]) + "]"

                    if mainWin.plotObject_1[mainWin.YIndexR[i]].lineStyle:
                        lineStyle = mainWin.plotObject_1[mainWin.YIndexR[i]].lineStyle
                    else:
                        lineStyle = 'solid'

                    if mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor:
                        lineColor = mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor
                    else:
                        pltColor = mainWin.colorSet[(mainWin.YIndexR[i])%mainWin.nColorSet]
                        lineColor = pltColor

                    lineWidth = mainWin.plotObject_1[mainWin.YIndexR[i]].width
                    drawStyle = mainWin.plotObject_1[mainWin.YIndexR[i]].drawStyle
                    label = mainWin.plotObject_1[mainWin.YIndexR[i]].label
                    marker = mainWin.plotObject_1[mainWin.YIndexR[i]].marker
                    markerSize = mainWin.plotObject_1[mainWin.YIndexR[i]].size
                    markerEdgeColor = mainWin.plotObject_1[mainWin.YIndexR[i]].edgeColor
                    markerFaceColor = mainWin.plotObject_1[mainWin.YIndexR[i]].faceColor

                    self.text += "\naxs[" + str(i_ax) + "].plot(x, y," + \
                         " color='" + str(lineColor) + "'," + \
                         " linestyle='" + str(lineStyle) + "'," + \
                         " linewidth=" + str(lineWidth) + "," + \
                         "\n  " + \
                         " drawstyle='" + str(drawStyle) + "'," + \
                         " label='" + str(label) + "'," + \
                         " marker='" + str(marker) + "'," + \
                         "\n  " + \
                         " markersize=" + str(markerSize) + "," + \
                         " markeredgecolor='" + str(markerEdgeColor) + "'," + \
                         " markerfacecolor='" + str(markerFaceColor) + "')"

                    if mainWin.plotObject_1[mainWin.YIndexR[i]].multiLinLog == 'linear':
                        self.text += "\naxs[" + str(i_ax) + "].ticklabel_format(axis='y', style='sci'," + \
                            " scilimits=(-2, 2),useMathText=True)"
                    i_ax += 1

                if (mainWin.axesPropObject_1.s_xMin.split()) or (mainWin.axesPropObject_1.s_xMax.split()):
                    self.text += "\n"
                    self.text += "\nfor ax in axs:"
                    if (mainWin.axesPropObject_1.s_xMin.split()):
                        self.text += "\n    ax.set_xlim(left =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_xMin)) + ")"
                    if (mainWin.axesPropObject_1.s_xMax.split()):
                        self.text += "\n    ax.set_xlim(right =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_xMax)) + ")"

                self.text += "\n"
                self.text += "\nfor ax in axs:"
                self.text += "\n    ax.legend(loc='best')"
                self.text += "\n"
                self.text += "\nplt.show()"
                self.text += "\n"
            else:
                self.text = "import matplotlib.pyplot as plt"
                self.text += "\nimport numpy as np"
                self.text += "\nreader = np.loadtxt('" + mainWin.fileName + "')"
                self.text += "\nfig, axs = plt.subplots(" + str(mainWin.n_variables) + \
                   ", 1, sharex=True)"
                self.text += "\nfig.subplots_adjust(hspace=0.0)"
                self.text += "\nfor ax in axs:"
                self.text += "\n    ax.label_outer"
                self.text += "\n    ax.grid(color='lightgrey')"
                self.text += "\n    ax.ticklabel_format(axis='x', style='sci'," + \
                    " scilimits=(-2,2), useMathText=True)"

                self.text += "\nx = reader[:, " + str(mainWin.XIndex) + "]"
                if mainWin.avgrmsObject_1.avg:
                    self.text += "\nreader_avg = np.loadtxt('" + mainWin.filename_avg + "')"
                    self.text += "\nt_avg = reader_avg[:, 0]"
                if mainWin.avgrmsObject_1.rms:
                    self.text += "\nreader_rms = np.loadtxt('" + mainWin.filename_rms + "')"
                    self.text += "\nt_rms = reader_rms[:, 0]"

                i_ax = 0
                for i in range(0,len(mainWin.YIndex)):
                    self.text += "\n\ny = reader[:, " + str(mainWin.YIndex[i]) + "]"
                    if mainWin.avgrmsObject_1.avg:
                        self.text += "\ny_avg = reader_avg[:, " + str(i_ax+1) + "]"
                    if mainWin.avgrmsObject_1.rms:
                        self.text += "\ny_rms = reader_rms[:, " + str(i_ax+1) + "]"

                    if mainWin.plotObject_1[mainWin.YIndex[i]].lineColor:
                        lineColor = mainWin.plotObject_1[mainWin.YIndex[i]].lineColor
                    else:
                        pltColor = mainWin.colorSet[(mainWin.YIndex[i])%mainWin.nColorSet]
                        lineColor = pltColor

                    label = mainWin.plotObject_1[mainWin.YIndex[i]].label
                    marker = mainWin.plotObject_1[mainWin.YIndex[i]].marker
                    markerSize = mainWin.plotObject_1[mainWin.YIndex[i]].size
                    markerEdgeColor = mainWin.plotObject_1[mainWin.YIndex[i]].edgeColor
                    markerFaceColor = mainWin.plotObject_1[mainWin.YIndex[i]].faceColor

                    self.text += "\naxs[" + str(i_ax) + "].plot(x, y," + \
                         " color='" + str(lineColor) + "'," + \
                         " linestyle='solid'," + \
                         " linewidth=1.0," + \
                         "\n  " + \
                         " drawstyle='default'," + \
                         " label='" + str(label) + "'," + \
                         " marker='" + str(marker) + "'," + \
                         "\n  " + \
                         " markersize=" + str(markerSize) + "," + \
                         " markeredgecolor='" + str(markerEdgeColor) + "'," + \
                         " markerfacecolor='" + str(markerFaceColor) + "')"
                    if mainWin.avgrmsObject_1.avg:
                        self.text += "\naxs[" + str(i_ax) + "].plot(t_avg, y_avg," + \
                             " color='" + str(lineColor) + "'," + \
                             " linestyle='dashed'," + \
                             " linewidth=1.0," + \
                             "\n  " + \
                             " drawstyle='default'," + \
                             " label='')"
                    if mainWin.avgrmsObject_1.rms:
                        self.text += "\naxs[" + str(i_ax) + "].plot(t_rms, y_rms," + \
                             " color='" + str(lineColor) + "'," + \
                             " linestyle='dashdot'," + \
                             " linewidth=1.0," + \
                             "\n  " + \
                             " drawstyle='default'," + \
                             " label='')"
                    self.text += "\naxs[" + str(i_ax) + "].ticklabel_format(axis='y', style='sci'," + \
                        " scilimits=(-2, 2),useMathText=True)"
                    i_ax += 1
                for i in range(0,len(mainWin.YIndexR)):
                    self.text += "\n\ny = reader[:, " + str(mainWin.YIndexR[i]) + "]"
                    if mainWin.avgrmsObject_1.avg:
                        self.text += "\ny_avg = reader_avg[:, " + str(i_ax+1) + "]"
                    if mainWin.avgrmsObject_1.rms:
                        self.text += "\ny_rms = reader_rms[:, " + str(i_ax+1) + "]"

                    if mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor:
                        lineColor = mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor
                    else:
                        pltColor = mainWin.colorSet[(mainWin.YIndexR[i])%mainWin.nColorSet]
                        lineColor = pltColor

                    label = mainWin.plotObject_1[mainWin.YIndexR[i]].label
                    marker = mainWin.plotObject_1[mainWin.YIndexR[i]].marker
                    markerSize = mainWin.plotObject_1[mainWin.YIndexR[i]].size
                    markerEdgeColor = mainWin.plotObject_1[mainWin.YIndexR[i]].edgeColor
                    markerFaceColor = mainWin.plotObject_1[mainWin.YIndexR[i]].faceColor

                    self.text += "\naxs[" + str(i_ax) + "].plot(x, y," + \
                         " color='" + str(lineColor) + "'," + \
                         " linestyle='solid'," + \
                         " linewidth=1.0," + \
                         "\n  " + \
                         " drawstyle='default'," + \
                         " label='" + str(label) + "'," + \
                         " marker='" + str(marker) + "'," + \
                         "\n  " + \
                         " markersize=" + str(markerSize) + "," + \
                         " markeredgecolor='" + str(markerEdgeColor) + "'," + \
                         " markerfacecolor='" + str(markerFaceColor) + "')"
                    if mainWin.avgrmsObject_1.avg:
                        self.text += "\naxs[" + str(i_ax) + "].plot(t_avg, y_avg," + \
                             " color='" + str(lineColor) + "'," + \
                             " linestyle='dashed'," + \
                             " linewidth=1.0," + \
                             "\n  " + \
                             " drawstyle='default'," + \
                             " label='')"
                    if mainWin.avgrmsObject_1.rms:
                        self.text += "\naxs[" + str(i_ax) + "].plot(t_rms, y_rms," + \
                             " color='" + str(lineColor) + "'," + \
                             " linestyle='dashdot'," + \
                             " linewidth=1.0," + \
                             "\n  " + \
                             " drawstyle='default'," + \
                             " label='')"
                    self.text += "\naxs[" + str(i_ax) + "].ticklabel_format(axis='y', style='sci'," + \
                        " scilimits=(-2, 2),useMathText=True)"
                    i_ax += 1

                if (mainWin.axesPropObject_1.s_xMin.split()) or (mainWin.axesPropObject_1.s_xMax.split()):
                    self.text += "\n"
                    self.text += "\nfor ax in axs:"
                    if (mainWin.axesPropObject_1.s_xMin.split()):
                        self.text += "\n    ax.set_xlim(left =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_xMin)) + ")"
                    if (mainWin.axesPropObject_1.s_xMax.split()):
                        self.text += "\n    ax.set_xlim(right =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_xMax)) + ")"

                self.text += "\n"
                self.text += "\nfor ax in axs:"
                self.text += "\n    ax.legend(loc='best')"
                self.text += "\n"
                self.text += "\nplt.show()"
                self.text += "\n"
        else:
            if not (mainWin.avgrmsObject_1.avg or mainWin.avgrmsObject_1.rms):
                self.text = "import matplotlib.pylab as plt"
                self.text += "\nfig, ax = plt.subplots()"

                if mainWin.ext == '.csv':
                    self.text += "\nimport pandas as pd"
                    self.text += "\nreader = pd.read_csv('"+mainWin.fileName+"')"
                if (mainWin.ext == '.dat')|(mainWin.ext == '.txt'):
                    self.text += "\nimport numpy as np"
                    self.text += "\nreader = np.loadtxt('"+mainWin.fileName+"')"
                    self.text += "\nls=[]"
                    self.text += "\nlbs=[]"
                if mainWin.YIndex != None:
                    for i in range(0,len(mainWin.YIndex)):
                        if mainWin.ext == '.csv':
                            self.text += "\nax.plot(reader.iloc[:," \
                                +str(mainWin.XIndex)+"],reader.iloc[:,"+str(mainWin.YIndex[i]) +"],"
                        elif (mainWin.ext == '.dat') | (mainWin.ext == '.txt') :
                            self.text += "\nl=ax.plot(reader[:," + str(mainWin.XIndex) + "],reader[:,"\
                               + str(mainWin.YIndex[i]) + "],"
                        self.text += "\n   color = '" +str(mainWin.plotObject_1[mainWin.YIndex[i]].lineColor)
                        self.text +="', linestyle ='" + str(mainWin.plotObject_1[mainWin.YIndex[i]].lineStyle)
                        self.text +="', linewidth = " + str(mainWin.plotObject_1[mainWin.YIndex[i]].width)
                        self.text +=",\n   drawstyle = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].drawStyle)
                        self.text +="', label = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].label)
                        self.text +="',\n   marker = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].marker)
                        self.text +="', markersize = " + str(mainWin.plotObject_1[mainWin.YIndex[i]].size)
                        self.text +=",markeredgecolor = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].edgeColor)
                        self.text +="',markerfacecolor = '" + \
                             str(mainWin.plotObject_1[mainWin.YIndex[i]].faceColor)+"')"
                        self.text +="\nls.append(l[0])"
                        self.text +="\nlbs.append('"+str(mainWin.plotObject_1[mainWin.YIndex[i]].label)+"')"
                if mainWin.YIndexR != None:
                    self.text = self.text+ "\nax2 = ax.twinx()"
                    for i in range(0,len(mainWin.YIndexR)):
                        if mainWin.ext == '.csv':
                            self.text += "\nl=ax2.plot(reader.iloc[:," \
                                + str(mainWin.XIndex)+"],reader.iloc[:,"+str(mainWin.YIndexR[i]) +"],"
                        elif (mainWin.ext == '.dat') | (mainWin.ext == '.txt') :
                            self.text += "\nl=ax2.plot(reader[:," + str(mainWin.XIndex) + "],reader[:," + \
                                str(mainWin.YIndexR[i]) + "],"
                        self.text += "\n   color = '" +str(mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor)
                        self.text +="', linestyle ='" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].lineStyle)
                        self.text +="', linewidth = " + str(mainWin.plotObject_1[mainWin.YIndexR[i]].width)
                        self.text +=",\n   drawstyle = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].drawStyle)
                        self.text +="', label = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].label)
                        self.text +="',\n   marker = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].marker)
                        self.text +="', markersize = " + str(mainWin.plotObject_1[mainWin.YIndexR[i]].size)
                        self.text +=",markeredgecolor = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].edgeColor)
                        self.text +="',markerfacecolor = '" + \
                            str(mainWin.plotObject_1[mainWin.YIndexR[i]].faceColor) + "')"
                        self.text +="\nls.append(l[0])"
                        self.text +="\nlbs.append('"+str(mainWin.plotObject_1[mainWin.YIndexR[i]].label)+"')"
                    if mainWin.axesPropObject_1.yScale2 == 'linear':
                        if len(mainWin.YIndexR)!=0:
                            self.text += "\nax2.ticklabel_format(axis='y', style='sci', scilimits=(" \
                            +str(mainWin.axesPropObject_1.ySNL2)+","
                            self.text += str(mainWin.axesPropObject_1.ySNU2)+"), useMathText=" + \
                                str(mainWin.axesPropObject_1.ySNM2)+")"
                        else:
                            self.text += "\nax2.ticklabel_format(axis='y', style='sci', scilimits=(" \
                            +str(mainWin.axesPropObject_1.ySNL)+","
                            self.text += str(mainWin.axesPropObject_1.ySNU)+"), useMathText=" + \
                                str(mainWin.axesPropObject_1.ySNM)+")"
                    if mainWin.axesPropObject_1.yScale2 != None and len(mainWin.YIndexR)!=0:
                        self.text += "\nax2.set_yscale('" + str(mainWin.axesPropObject_1.yScale2) + "')"
                    else:
                        self.text += "\nax2.set_yscale('" + str(mainWin.axesPropObject_1.yScale) + "')"
                    if mainWin.axesPropObject_1.yLabel2 != None:
                        self.text += "\nax2.set_ylabel('" + str(mainWin.axesPropObject_1.yLabel2) + "')"

                if mainWin.axesPropObject_1.xScale != None:
                    self.text += "\nax.set_xscale('" + str(mainWin.axesPropObject_1.xScale) + "')"
                if mainWin.axesPropObject_1.yScale != None and len(mainWin.YIndex)!=0:
                    self.text += "\nax.set_yscale('" + str(mainWin.axesPropObject_1.yScale) + "')"
                else:
                    self.text += "\nax.set_yscale('" + str(mainWin.axesPropObject_1.yScale2) + "')"
                if mainWin.axesPropObject_1.xLabel != None:
                    self.text += "\nax.set_xlabel('" + str(mainWin.axesPropObject_1.xLabel) + "')"
                if mainWin.axesPropObject_1.yLabel != None:
                    self.text += "\nax.set_ylabel('" + str(mainWin.axesPropObject_1.yLabel) + "')"

                if (mainWin.axesPropObject_1.s_xMin.split()):
                    self.text += "\nax.set_xlim(left =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMin)) + ")"
                if (mainWin.axesPropObject_1.s_xMax.split()):
                    self.text += "\nax.set_xlim(right =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMax)) + ")"

                if len(mainWin.YIndex) != 0:
                    if (mainWin.axesPropObject_1.s_yMin.split()):
                        self.text += "\nax.set_ylim(bottom =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMin)) + ")"
                    if (mainWin.axesPropObject_1.s_yMax.split()):
                        self.text += "\nax.set_ylim(top =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMax)) + ")"
                if len(mainWin.YIndexR) != 0:
                    if (mainWin.axesPropObject_1.s_yMin2.split()):
                        self.text += "\nax2.set_ylim(bottom =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMin2)) + ")"
                    if (mainWin.axesPropObject_1.s_yMax2.split()):
                        self.text += "\nax2.set_ylim(top =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMax2)) + ")"
                if (len(mainWin.YIndex) != 0) and (len(mainWin.YIndexR) == 0):
                    self.text += "\ny_min, y_max = ax.get_ylim()"
                    self.text += "\nax2.set_ylim(bottom=y_min, top=y_max)"
                if (len(mainWin.YIndex) == 0) and (len(mainWin.YIndexR) != 0):
                    self.text += "\ny_min, y_max = ax2.get_ylim()"
                    self.text += "\nax.set_ylim(bottom=y_min, top=y_max)"

                if mainWin.gridObject_1.gridEnable:
                    self.text =  self.text + "\nax.grid(color ='" + str(mainWin.gridObject_1.lineColor)
                    self.text += "', linestyle = '" + str(mainWin.gridObject_1.lineStyle)
                    self.text +="',axis = '" + str(mainWin.gridObject_1.axis)
                    self.text +="',which = '" + str(mainWin.gridObject_1.which)
                    self.text += "', linewidth = "+ str(mainWin.gridObject_1.width) + ")"

                if mainWin.legendEnable:
                    if len(mainWin.YIndex)>0 and len(mainWin.YIndexR)==0:
                        self.text += "\nax.legend(loc = '" + mainWin.legendObject_1.location
                        self.text += "',frameon = " + str(mainWin.legendObject_1.frameon)
                        self.text += ", fontsize = " + str(mainWin.legendObject_1.fontsize)
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                    elif len(mainWin.YIndexR)>0 and len(mainWin.YIndex)==0:
                        self.text += "\nax2.legend(loc = '" + mainWin.legendObject_1.location
                        self.text += "',frameon = " + str(mainWin.legendObject_1.frameon)
                        self.text += ", fontsize = " + str(mainWin.legendObject_1.fontsize)
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                    elif len(mainWin.YIndex) > 0 and len(mainWin.YIndexR) > 0:
                        self.text = self.text+ "\nax.legend(ls,lbs"
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                if mainWin.titleObject_1.titleEnable:
                    self.text += "\nax.set_title(label = '" + str(mainWin.titleObject_1.label)
                    self.text += "',loc = '" +str(mainWin.titleObject_1.loc) + "')"
                if mainWin.axesPropObject_1.xScale == 'linear':
                    self.text += "\nax.ticklabel_format(axis='x', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.xSNL)+","
                    self.text += str(mainWin.axesPropObject_1.xSNU)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.xSNM)+")"
                if mainWin.axesPropObject_1.yScale == 'linear' and len(mainWin.YIndex)!=0:
                    self.text += "\nax.ticklabel_format(axis='y', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.ySNL)+","
                    self.text += str(mainWin.axesPropObject_1.ySNU)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.ySNM)+")"
                else:
                    self.text += "\nax.ticklabel_format(axis='y', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.ySNL2)+","
                    self.text += str(mainWin.axesPropObject_1.ySNU2)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.ySNM2)+")"
                self.text += "\nplt.tight_layout()"
                self.text += "\nplt.show()"
                self.text +="\n"
            else:
                if (mainWin.axesPropObject_1.yScale != 'linear') or \
                   (mainWin.axesPropObject_1.yScale2 != 'linear') or \
                   (mainWin.axesPropObject_1.xScale != 'linear'):
                    mainWin.displayMessage1("Axis Error", "log axis is not allowed with avg/rms.")
                    print("ScriptObject(QWidget): InitWindow: log axis not allowed. Halting...")
                    sys.exit()

                self.text = "import matplotlib.pylab as plt"
                self.text += "\nfig, ax = plt.subplots()"

                if mainWin.ext == '.csv':
                    self.text += "\nimport pandas as pd"
                    self.text += "\nreader = pd.read_csv('"+mainWin.fileName+"')"
                if (mainWin.ext == '.dat')|(mainWin.ext == '.txt'):
                    self.text += "\nimport numpy as np"
                    self.text += "\nreader = np.loadtxt('"+mainWin.fileName+"')"
                    self.text += "\nls=[]"
                    self.text += "\nlbs=[]"
                if mainWin.avgrmsObject_1.avg:
                    self.text += "\nreader_avg = np.loadtxt('" + mainWin.filename_avg + "')" 
                    self.text += "\nt_avg = reader_avg[:, 0]"
                if mainWin.avgrmsObject_1.rms:
                    self.text += "\nreader_rms = np.loadtxt('" + mainWin.filename_rms + "')" 
                    self.text += "\nt_rms = reader_rms[:, 0]"

                if mainWin.YIndex != None:
                    i_ax = 0
                    for i in range(0,len(mainWin.YIndex)):
                        if mainWin.avgrmsObject_1.avg:
                            self.text += "\ny_avg = reader_avg[:, " + str(i_ax+1) + "]"
                        if mainWin.avgrmsObject_1.rms:
                            self.text += "\ny_rms = reader_rms[:, " + str(i_ax+1) + "]"

                        if mainWin.ext == '.csv':
                            self.text += "\nax.plot(reader.iloc[:," \
                                +str(mainWin.XIndex)+"],reader.iloc[:,"+str(mainWin.YIndex[i]) +"],"
                        elif (mainWin.ext == '.dat') | (mainWin.ext == '.txt') :
                            self.text += "\nl=ax.plot(reader[:," + str(mainWin.XIndex) + "],reader[:,"\
                               + str(mainWin.YIndex[i]) + "],"
                        self.text += "\n   color = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].lineColor)
                        self.text +="', linestyle='solid"
                        self.text +="', linewidth=1.0"
                        self.text +=",\n   drawstyle='default"
                        self.text +="', label = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].label)
                        self.text +="',\n   marker = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].marker)
                        self.text +="', markersize = " + str(mainWin.plotObject_1[mainWin.YIndex[i]].size)
                        self.text +=",markeredgecolor = '" + str(mainWin.plotObject_1[mainWin.YIndex[i]].edgeColor)
                        self.text +="',markerfacecolor = '" + \
                             str(mainWin.plotObject_1[mainWin.YIndex[i]].faceColor)+"')"
                        self.text +="\nls.append(l[0])"
                        self.text +="\nlbs.append('"+str(mainWin.plotObject_1[mainWin.YIndex[i]].label)+"')"

                        if mainWin.avgrmsObject_1.avg:
                            self.text += "\nax.plot(t_avg, y_avg," + \
                                 " color='" + str(mainWin.plotObject_1[mainWin.YIndex[i]].lineColor) + "'," + \
                                 " linestyle='dashed'," + \
                                 " linewidth=1.0," + \
                                 "\n  " + \
                                 " drawstyle='default'," + \
                                 " label='')"
                        if mainWin.avgrmsObject_1.rms:
                            self.text += "\nax.plot(t_rms, y_rms," + \
                                 " color='" + str(mainWin.plotObject_1[mainWin.YIndex[i]].lineColor) + "'," + \
                                 " linestyle='dashdot'," + \
                                 " linewidth=1.0," + \
                                 "\n  " + \
                                 " drawstyle='default'," + \
                                 " label='')"
                        i_ax += 1

                if mainWin.YIndexR != None:
                    self.text = self.text+ "\nax2 = ax.twinx()"
                    for i in range(0,len(mainWin.YIndexR)):
                        if mainWin.avgrmsObject_1.avg:
                            self.text += "\ny_avg = reader_avg[:, " + str(i_ax+1) + "]"
                        if mainWin.avgrmsObject_1.rms:
                            self.text += "\ny_rms = reader_rms[:, " + str(i_ax+1) + "]"

                        if mainWin.ext == '.csv':
                            self.text += "\nl=ax2.plot(reader.iloc[:," \
                                + str(mainWin.XIndex)+"],reader.iloc[:,"+str(mainWin.YIndexR[i]) +"],"
                        elif (mainWin.ext == '.dat') | (mainWin.ext == '.txt') :
                            self.text += "\nax2.plot(reader[:," + str(mainWin.XIndex) + "],reader[:," + \
                                str(mainWin.YIndexR[i]) + "],"
                        self.text += "\n   color = '" +str(mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor)
                        self.text +="', linestyle='solid"
                        self.text +="', linewidth=1.0"
                        self.text +=",\n   drawstyle='default"
                        self.text +="', label = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].label)
                        self.text +="',\n   marker = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].marker)
                        self.text +="', markersize = " + str(mainWin.plotObject_1[mainWin.YIndexR[i]].size)
                        self.text +=",markeredgecolor = '" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].edgeColor)
                        self.text +="',markerfacecolor = '" + \
                            str(mainWin.plotObject_1[mainWin.YIndexR[i]].faceColor) + "')"
                        self.text +="\nls.append(l[0])"
                        self.text +="\nlbs.append('"+str(mainWin.plotObject_1[mainWin.YIndexR[i]].label)+"')"

                        if mainWin.avgrmsObject_1.avg:
                            self.text += "\nax2.plot(t_avg, y_avg," + \
                                 " color='" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor) + "'," + \
                                 " linestyle='dashed'," + \
                                 " linewidth=1.0," + \
                                 "\n  " + \
                                 " drawstyle='default'," + \
                                 " label='')"
                        if mainWin.avgrmsObject_1.rms:
                            self.text += "\nax2.plot(t_rms, y_rms," + \
                                 " color='" + str(mainWin.plotObject_1[mainWin.YIndexR[i]].lineColor) + "'," + \
                                 " linestyle='dashdot'," + \
                                 " linewidth=1.0," + \
                                 "\n  " + \
                                 " drawstyle='default'," + \
                                 " label='')"
                        i_ax += 1

                    if mainWin.axesPropObject_1.yScale2 == 'linear':
                        if len(mainWin.YIndexR)!=0:
                            self.text += "\nax2.ticklabel_format(axis='y', style='sci', scilimits=(" \
                            +str(mainWin.axesPropObject_1.ySNL2)+","
                            self.text += str(mainWin.axesPropObject_1.ySNU2)+"), useMathText=" + \
                                str(mainWin.axesPropObject_1.ySNM2)+")"
                        else:
                            self.text += "\nax2.ticklabel_format(axis='y', style='sci', scilimits=(" \
                            +str(mainWin.axesPropObject_1.ySNL)+","
                            self.text += str(mainWin.axesPropObject_1.ySNU)+"), useMathText=" + \
                                str(mainWin.axesPropObject_1.ySNM)+")"
                    if mainWin.axesPropObject_1.yScale2 != None and len(mainWin.YIndexR)!=0:
                        self.text += "\nax2.set_yscale('" + str(mainWin.axesPropObject_1.yScale2) + "')"
                    else:
                        self.text += "\nax2.set_yscale('" + str(mainWin.axesPropObject_1.yScale) + "')"
                    if mainWin.axesPropObject_1.yLabel2 != None:
                        self.text += "\nax2.set_ylabel('" + str(mainWin.axesPropObject_1.yLabel2) + "')"

                if mainWin.axesPropObject_1.xScale != None:
                    self.text += "\nax.set_xscale('" + str(mainWin.axesPropObject_1.xScale) + "')"
                if mainWin.axesPropObject_1.yScale != None and len(mainWin.YIndex)!=0:
                    self.text += "\nax.set_yscale('" + str(mainWin.axesPropObject_1.yScale) + "')"
                else:
                    self.text += "\nax.set_yscale('" + str(mainWin.axesPropObject_1.yScale2) + "')"
                if mainWin.axesPropObject_1.xLabel != None:
                    self.text += "\nax.set_xlabel('" + str(mainWin.axesPropObject_1.xLabel) + "')"
                if mainWin.axesPropObject_1.yLabel != None:
                    self.text += "\nax.set_ylabel('" + str(mainWin.axesPropObject_1.yLabel) + "')"

                if (mainWin.axesPropObject_1.s_xMin.split()):
                    self.text += "\nax.set_xlim(left =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMin)) + ")"
                if (mainWin.axesPropObject_1.s_xMax.split()):
                    self.text += "\nax.set_xlim(right =" + \
                       "%11.4E"%(float(mainWin.axesPropObject_1.s_xMax)) + ")"

                if len(mainWin.YIndex) != 0:
                    if (mainWin.axesPropObject_1.s_yMin.split()):
                        self.text += "\nax.set_ylim(bottom =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMin)) + ")"
                    if (mainWin.axesPropObject_1.s_yMax.split()):
                        self.text += "\nax.set_ylim(top =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMax)) + ")"
                if len(mainWin.YIndexR) != 0:
                    if (mainWin.axesPropObject_1.s_yMin2.split()):
                        self.text += "\nax2.set_ylim(bottom =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMin2)) + ")"
                    if (mainWin.axesPropObject_1.s_yMax2.split()):
                        self.text += "\nax2.set_ylim(top =" + \
                           "%11.4E"%(float(mainWin.axesPropObject_1.s_yMax2)) + ")"
                if (len(mainWin.YIndex) != 0) and (len(mainWin.YIndexR) == 0):
                    self.text += "\ny_min, y_max = ax.get_ylim()"
                    self.text += "\nax2.set_ylim(bottom=y_min, top=y_max)"
                if (len(mainWin.YIndex) == 0) and (len(mainWin.YIndexR) != 0):
                    self.text += "\ny_min, y_max = ax2.get_ylim()"
                    self.text += "\nax.set_ylim(bottom=y_min, top=y_max)"

                if mainWin.gridObject_1.gridEnable:
                    self.text =  self.text + "\nax.grid(color ='" + str(mainWin.gridObject_1.lineColor)
                    self.text += "', linestyle = '" + str(mainWin.gridObject_1.lineStyle)
                    self.text +="',axis = '" + str(mainWin.gridObject_1.axis)
                    self.text +="',which = '" + str(mainWin.gridObject_1.which)
                    self.text += "', linewidth = "+ str(mainWin.gridObject_1.width) + ")"

                if mainWin.legendEnable:
                    if len(mainWin.YIndex)>0 and len(mainWin.YIndexR)==0:
                        self.text += "\nax.legend(loc = '" + mainWin.legendObject_1.location
                        self.text += "',frameon = " + str(mainWin.legendObject_1.frameon)
                        self.text += ", fontsize = " + str(mainWin.legendObject_1.fontsize)
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                    elif len(mainWin.YIndexR)>0 and len(mainWin.YIndex)==0:
                        self.text += "\nax2.legend(loc = '" + mainWin.legendObject_1.location
                        self.text += "',frameon = " + str(mainWin.legendObject_1.frameon)
                        self.text += ", fontsize = " + str(mainWin.legendObject_1.fontsize)
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                    elif len(mainWin.YIndex) > 0 and len(mainWin.YIndexR) > 0:
                        self.text = self.text+ "\nax.legend(ls,lbs"
                        if mainWin.legendObject_1.title == None:
                            self.text += ", title = " + 'None'
                        else:
                            self.text += ", title = '" + mainWin.legendObject_1.title + "'"
                        self.text += ",\n   markerfirst = " + str(mainWin.legendObject_1.markerfirst)
                        self.text += ", markerscale = " + str(mainWin.legendObject_1.markerscale)
                        self.text += ", labelspacing = " + str(mainWin.legendObject_1.labelspacing)
                        self.text += ", columnspacing = "+ str(mainWin.legendObject_1.columnspacing)+")"
                if mainWin.titleObject_1.titleEnable:
                    self.text += "\nax.set_title(label = '" + str(mainWin.titleObject_1.label)
                    self.text += "',loc = '" +str(mainWin.titleObject_1.loc) + "')"
                if mainWin.axesPropObject_1.xScale == 'linear':
                    self.text += "\nax.ticklabel_format(axis='x', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.xSNL)+","
                    self.text += str(mainWin.axesPropObject_1.xSNU)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.xSNM)+")"
                if mainWin.axesPropObject_1.yScale == 'linear' and len(mainWin.YIndex)!=0:
                    self.text += "\nax.ticklabel_format(axis='y', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.ySNL)+","
                    self.text += str(mainWin.axesPropObject_1.ySNU)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.ySNM)+")"
                else:
                    self.text += "\nax.ticklabel_format(axis='y', style='sci', scilimits=(" + \
                        str(mainWin.axesPropObject_1.ySNL2)+","
                    self.text += str(mainWin.axesPropObject_1.ySNU2)+"), useMathText=" + \
                        str(mainWin.axesPropObject_1.ySNM2)+")"
                self.text += "\nplt.show()"
                self.text +="\n"

        self.plainText.appendPlainText(self.text)
        self.plainText.setUndoRedoEnabled(False)
        vbox.addWidget(self.plainText)

    def openFileSaveDialog(self):
        options = QFileDialog.Option.DontUseNativeDialog

        s, _ = QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()", 
            ""," ", options=options)
        s1 = s.replace(' ', '')
        if ('.py' in s1):
            self.fileName = s1
            print('openFileSaveDialog: fileName:', self.fileName)
            f= open(os.path.expanduser(self.fileName),"w+")
            f.write(self.text)
            f.close();
        else:
            mainWin.displayMessage1("File Error", "file name must end in .py")

class PlotObject(object):
    def __init__(self, label = '',lineStyle = 'solid', drawStyle = 'default', width = 1.0, lineColor = None,
                        marker = '', size = 3, edgeColor = 'mediumblue',faceColor = 'white', multiLinLog = 'linear'):
        self.lineStyle =lineStyle; self.drawStyle = drawStyle;
        self.width = width; self.lineColor = lineColor;
        self.marker = marker; self.size = size;
        self.edgeColor = edgeColor;self.faceColor = faceColor;
        self.label = label;
        self.multiLinLog = multiLinLog;
    def setLineStyle(self,lineStyle):
        self.lineStyle = lineStyle;
    def setDrawStyle(self,drawStyle):
        self.drawStyle = drawStyle;
    def setWidth(self,width):
        self.width =  width;
    def setLineColor(self,lineColor):
        self.lineColor = lineColor;
    def setMarker(self,marker):
        self.marker = marker;
    def setSize(self,size):
        self.size = size;
    def setEdgeColor(self,edgeColor):
        self.edgeColor = edgeColor
    def setFaceColor(self,faceColor):
        self.faceColor = faceColor
    def setLabel(self,label):
        self.label = label;
    def setMultiLinLog(self,multiLinLog):
        self.multiLinLog = multiLinLog;

class TitleObject(object):
    def __init__(self, label = "", loc = 'center',titleEnable=True):
        self.label = label; self.loc = loc;
        self.titleEnable = titleEnable
    def setLabel(self,label):
        self.label = label;
    def setTitleEnable(self,titleEnable):
        self.titleEnable = titleEnable
    def setLoc(self,loc):
        self.loc = loc;

class GridObject(object):
    def __init__(self, lineStyle = 'solid', width = 0.7, lineColor = 'lightgrey',
           which = 'both', axis = 'both', gridEnable = True):
        self.lineStyle =lineStyle;
        self.width = width; self.lineColor = lineColor;
        self.which = which; self.axis = axis;
        self.gridEnable = True;
    def setLineStyle(self,lineStyle):
        self.lineStyle = lineStyle;
    def setWidth(self,width):
        self.width =  width;
    def setLineColor(self,lineColor):
        self.lineColor = lineColor;
    def setWhich(self,which):
        self.which = which;
    def setAxis(self,axis):
        self.axis = axis
    def setGridEnable(self,gridEnable):
        self.gridEnable = gridEnable;

class MultiPlotObject(object):
    def __init__(self, multiPlot=False):
        self.multiPlot = multiPlot;
    def setMultiPlot(self, multiPlot):
        self.multiPlot = multiPlot;

class FourierObject(object):
    def __init__(self, fourier=False, nFourier=10, tStart=0.0, tEnd=0.0):
        self.fourier = fourier;
        self.nFourier = nFourier;
        self.tStart = tStart;
        self.tEnd = tEnd;
    def setFourier(self, fourier):
        self.fourier = fourier;
    def setNFourier(self, nFourier):
        self.nFourier = nFourier;
    def setTStart(self, tStart):
        self.tStart = tStart;
    def setTEnd(self, tEnd):
        self.tEnd = tEnd;

class AvgRmsObject(object):
    def __init__(self, avg=False, rms=False, period=0.0):
        self.avg = avg;
        self.rms = rms;
        self.period = period;
    def setAvg(self, avg):
        self.avg = avg;
    def setRms(self, rms):
        self.rms = rms;
    def setPeriod(self, period):
        self.period = period;

class PowerObject(object):
    def __init__(self, compute_power=False, v_string='', i_string='', period=0.0):
        self.compute_power = compute_power;
        self.v_string = v_string;
        self.i_string = i_string;
        self.period = period;
    def setComputePower(self, compute_power):
        self.compute_power = compute_power;
    def setVString(self, v_string):
        self.v_string = v_string;
    def setIString(self, i_string):
        self.i_string = i_string;
    def setPeriod(self, period):
        self.period = period;

class TicksPropObject(object):
    def __init__(self,xTicksEnable = True, direction='out',rotation=0,xticks = [],xtickslabels=[]):
        self.direction = direction;self.rotation = rotation;
        self.xticks = xticks; self.xtickslabels = xtickslabels;
        self.xTicksEnable = xTicksEnable
    def setxTicksEnable(self,xTicksEnable):
        self.xTicksEnable = xTicksEnable;
    def setDirection(self,direction):
        self.direction = direction;
    def setRotation(self,rotation):
        self.rotation = rotation;
    def setXTicks(self,xticks):
        self.xticks = xticks;
    def setXTicksLabels(self,xtickslabels):
        self.xtickslabels = xtickslabels;

class AxesPropObject(object):
    def __init__(self,
           xScale='linear', xLabel='X-Axis', s_xMin='', s_xMax='', xSNL=-3, xSNU=3, xSNM=True,
           yScale='linear', yLabel='Y-Axis', s_yMin='', s_yMax='', ySNL=-3, ySNU=3, ySNM=True,
           yScale2='linear', yLabel2='Y-Axis', s_yMin2='', s_yMax2='', ySNL2=-3, ySNU2=3, ySNM2=True):
        self.xScale = xScale;self.xLabel = xLabel;
        self.s_xMin = s_xMin; self.s_xMax = s_xMax;
        self.yScale = yScale; self.yLabel = yLabel;
        self.s_yMin = s_yMin; self.s_yMax = s_yMax
        self.yScale2 = yScale2; self.yLabel2 = yLabel2;
        self.s_yMin2 = s_yMin2; self.s_yMax2 = s_yMax2
        self.ySNL = ySNL; self.xSNL = xSNL
        self.ySNU = ySNU; self.xSNU = xSNU
        self.ySNM = ySNM; self.xSNM = xSNM
        self.ySNL2 = ySNL2;
        self.ySNU2 = ySNU2;
        self.ySNM2 = ySNM2;
    def setxScale(self, xScale):
        self.xScale = xScale;
    def setxLabel(self, xLabel):
        self.xLabel = xLabel;
    def setxMin(self, s_xMin):
        self.s_xMin = s_xMin;
    def setxMax(self, s_xMax):
        self.s_xMax = s_xMax;
    def setyScale(self, yScale):
        self.yScale = yScale;
    def setyLabel(self, yLabel):
        self.yLabel = yLabel;
    def setyMin(self, s_yMin):
        self.s_yMin = s_yMin;
    def setyMax(self, s_yMax):
        self.s_yMax = s_yMax;
    def setyScale2(self, yScale2):
        self.yScale2 = yScale2;
    def setyLabel2(self, yLabel2):
        self.yLabel2 = yLabel2;
    def setyMin2(self, s_yMin2):
        self.s_yMin2 = s_yMin2;
    def setyMax2(self, s_yMax2):
        self.s_yMax2 = s_yMax2;
    def setySNM(self, ySNM):
        self.ySNM = ySNM;
    def setySNM2(self, ySNM2):
        self.ySNM2 = ySNM2;
    def setxSNM(self, xSNM):
        self.xSNM2 = xSNM;
    def setySNL(self, ySNL):
        self.ySNL = ySNL;
    def setySNL2(self, ySNL2):
        self.ySNL2 = ySNL2;
    def setxSNL(self, xSNL):
        self.xSNL = xSNL;
    def setySNU(self, ySNU):
        self.ySNU = ySNU;
    def setySNU2(self, ySNU2):
        self.ySNU2 = ySNU2;
    def setxSNU(self, xSNU):
        self.xSNU = xSNU;

class LegendObject(object):
    def __init__(self, location = 'best',frameon = True, fontsize = 10, title = None,
           markerfirst = True, markerscale = 1.0, labelspacing = 0.5, columnspacing = 2.0):
        self.location = location; self.frameon = frameon;
        self.fontsize = fontsize; self.title = title;
        self.markerfirst = markerfirst; self.markerscale = markerscale;
        self.labelspacing = labelspacing; self.columnspacing = columnspacing
    def setlocation(self,location):
        self.location = location;
    def setframeon(self,frameon):
        self.frameon = frameon;
    def setfontsize(self,fontsize):
        self.fontsize =  fontsize;
    def settitle(self,title):
        self.title = title;
    def setmarkerfirst(self,markerfirst):
        self.markerfirst = markerfirst;
    def setmarkerscale(self,markerscale):
        self.markerscale = markerscale;
    def setlabelspacing(self,labelspacing):
        self.labelspacing = labelspacing
    def setcolumnspacing(self,columnspacing):
        self.columnspacing = columnspacing

class LinePropPopup(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        canvas = QPixmap(185, 130)
        canvas.fill(QColor("#FFFFFF"));
        myFont = QFont(); myFont.setBold(True);

        self.frame1 = QFrame(self);self.frame1.setGeometry(QRect(10, 10, 250, 25))
        self.combo1 = QComboBox(self.frame1)
        self.combo1.currentIndexChanged.connect(self.yLineProp)

        self.frameD = QFrame(self);self.frameD.setGeometry(QRect(10, 175, 50, 25))
        self.def1 = QLabel("Default:",self.frameD);self.def1.setFont(myFont)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(65, 175, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        self.cb1.stateChanged.connect(self.dafaultLineProp)

        self.frame30 = QFrame(self);self.frame30.setGeometry(QRect(120, 1, 175, 25))
        self.leglabelT = QLabel("multi: lin/log",self.frame30);
        self.frame31 = QFrame(self);self.frame31.setGeometry(QRect(120, 15, 175, 25))
        self.combo5 = QComboBox(self.frame31)
        self.combo5.addItem("linear"); self.combo5.addItem("log")

        self.frame29 = QFrame(self);self.frame29.setGeometry(QRect(220,1,175,25))
        self.leglabelT = QLabel("Label:(used for legend)",self.frame29);
        self.frame28 = QFrame(self);self.frame28.setGeometry(QRect(220,15,175,25))
        self.leglabel = QLineEdit("",self.frame28);
        self.leglabel.setFixedWidth(120)

        self.frame2 = QFrame(self);self.frame2.setGeometry(QRect(20, 45, 250, 25))
        self.Label1 = QLabel("Line Properties:",self.frame2);
        myFont = QFont(); myFont.setBold(True);self.Label1.setFont(myFont)

        self.frame3 = QFrame(self);self.frame3.setGeometry(QRect(20, 65, 250, 25))
        self.Label2 = QLabel("Line Style:",self.frame3)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(90, 65, 250, 25))
        self.combo2 = QComboBox(self.frame7)
        self.combo2.addItem("solid");self.combo2.addItem("None");self.combo2.addItem("dotted")
        self.combo2.addItem("dashed");self.combo2.addItem("dashdot");

        self.frame4 = QFrame(self);self.frame4.setGeometry(QRect(20, 90, 250, 25))
        self.Label3 = QLabel("Draw Style:",self.frame4)
        self.frame8 = QFrame(self);self.frame8.setGeometry(QRect(90, 90, 250, 25))
        self.combo3 = QComboBox(self.frame8)
        self.combo3.addItem("default");self.combo3.addItem("steps-post");
        self.combo3.addItem("steps-pre");self.combo3.addItem("steps-mid")

        self.frame5 = QFrame(self);self.frame5.setGeometry(QRect(20, 115, 250, 25))
        self.Label4 = QLabel("Width:",self.frame5)
        self.frame26 = QFrame(self);self.frame26.setGeometry(QRect(90,115,115,25))
        self.wdBtn = QLineEdit("0.5",self.frame26);
        self.wdBtn.setFixedWidth(95)
        validator = QDoubleValidator()
        self.wdBtn.setValidator(validator)

        self.frame6 = QFrame(self);self.frame6.setGeometry(QRect(20, 140, 250, 25))
        self.Label5 = QLabel("Color:",self.frame6)
        self.frame25 = QFrame(self);self.frame25.setGeometry(QRect(100,140,30,25))
        self.lnBtn = QPushButton(self.frame25);self.lnBtn.clicked.connect(self.openlnColorDlg)
        self.pixmap = QPixmap(10,10);self.pixmap.fill(QColor("mediumblue"));
        self.lnCIcon= QIcon(self.pixmap);self.lnBtn.setIcon(self.lnCIcon);

        self.frame11 = QFrame(self);self.frame11.setGeometry(QRect(220, 45, 250, 25))
        self.Label6 = QLabel("Marker Properties:",self.frame11);self.Label6.setFont(myFont)

        self.frame12 = QFrame(self);self.frame12.setGeometry(QRect(220, 65, 250, 25))
        self.Label7 = QLabel("Style:",self.frame12)
        self.frame13 = QFrame(self);self.frame13.setGeometry(QRect(300, 65, 250, 25))
        self.combo4 = QComboBox(self.frame13)
        mStyles = ["", ".", "o", "x", "+", "D", "d", "s", "*", "v", "^", "<", ">",
            "h", "H", "X", "p", "P", ]
        for i in range(0,len(mStyles)):
            self.combo4.addItem(mStyles[i]);

#       self.frame14 = QFrame(self);self.frame14.setGeometry(QRect(220, 90, 250, 25))
        self.frame14 = QFrame(self);self.frame14.setGeometry(QRect(220, 90, 100, 25))
        self.Label8 = QLabel("Size:",self.frame14)
        self.frame27 = QFrame(self);self.frame27.setGeometry(QRect(300,90,115,25))
        self.sizeBtn = QLineEdit("0.5",self.frame27);
        self.sizeBtn.setFixedWidth(75)
        validator = QDoubleValidator()
        self.sizeBtn.setValidator(validator)

        self.frame16 = QFrame(self);self.frame16.setGeometry(QRect(220, 115, 250, 25))
        self.Label9 = QLabel("Edge Color:",self.frame16)
        self.frame24 = QFrame(self);self.frame24.setGeometry(QRect(325,115,30,25))
        self.edBtn = QPushButton(self.frame24);self.edBtn.clicked.connect(self.openEdColorDlg)
        self.pixmap = QPixmap(10,10);self.pixmap.fill(QColor("black"));
        self.edCIcon= QIcon(self.pixmap);self.edBtn.setIcon(self.edCIcon);

        self.frame17 = QFrame(self);self.frame17.setGeometry(QRect(220, 140, 250, 25))
        self.Label10 = QLabel("Face Color:",self.frame17)
        self.frame23 = QFrame(self);self.frame23.setGeometry(QRect(325,140,30,25))
        self.fcBtn = QPushButton(self.frame23);self.fcBtn.clicked.connect(self.openFcColorDlg)
        self.pixmap.fill(QColor("white"));
        self.redIcon= QIcon(self.pixmap);self.fcBtn.setIcon(self.redIcon);

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(120, 170, 90, 25))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(210, 170, 90, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(300, 170, 90, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)

        self.edgecolor = QColor('black');

        self.linecolor = QColor('mediumblue');
        self.facecolor = QColor('white');

    def applyBtnAction(self):
        if self.combo1.currentText()!= '':
            linePropSelect = self.combo1.currentText()
            selIndexL = mainWin.YCols.indexFromItem(mainWin.YCols.findItems(linePropSelect,Qt.MatchFlag.MatchContains)[0])
            selIndex = int(selIndexL.row())
            mainWin.plotObject_1[selIndex].setLineStyle(self.combo2.currentText())
            mainWin.plotObject_1[selIndex].setDrawStyle(self.combo3.currentText())
            mainWin.plotObject_1[selIndex].setWidth(float(self.wdBtn.text()))
            mainWin.plotObject_1[selIndex].setMarker(self.combo4.currentText())
            mainWin.plotObject_1[selIndex].setLineColor(self.linecolor.name())
            mainWin.plotObject_1[selIndex].setSize(float(self.sizeBtn.text()))
            mainWin.plotObject_1[selIndex].setFaceColor(self.facecolor.name())
            mainWin.plotObject_1[selIndex].setEdgeColor(self.edgecolor.name())
            mainWin.plotObject_1[selIndex].setLabel(self.leglabel.text())
            mainWin.plotObject_1[selIndex].setMultiLinLog(self.combo5.currentText())

            mainWin.plotDataWithChangedOptions()
    def yLineProp(self):
        linePropSelect = self.combo1.currentText()
        selIndexL = mainWin.YCols.indexFromItem(mainWin.YCols.findItems(linePropSelect,Qt.MatchFlag.MatchContains)[0])
        selIndex = int(selIndexL.row())
        self.combo2.setCurrentText(mainWin.plotObject_1[selIndex].lineStyle)
        self.combo3.setCurrentText(mainWin.plotObject_1[selIndex].drawStyle)
        self.wdBtn.setText(str(mainWin.plotObject_1[selIndex].width))
        self.sizeBtn.setText(str(mainWin.plotObject_1[selIndex].size))
        if mainWin.plotObject_1[selIndex].lineColor:
            self.pixmap.fill(QColor(mainWin.plotObject_1[selIndex].lineColor));
        else:
            print('Line color missing')
        self.redIcon= QIcon(self.pixmap);
        self.lnBtn.setIcon(self.redIcon);
        self.combo4.setCurrentText(mainWin.plotObject_1[selIndex].marker)
        self.pixmap.fill(QColor(mainWin.plotObject_1[selIndex].faceColor));
        self.redIcon= QIcon(self.pixmap);
        self.fcBtn.setIcon(self.redIcon);
        self.pixmap.fill(QColor(mainWin.plotObject_1[selIndex].edgeColor));
        self.redIcon= QIcon(self.pixmap);
        self.edBtn.setIcon(self.redIcon);
        self.leglabel.setText(str(mainWin.plotObject_1[selIndex].label))
        if mainWin.plotObject_1[selIndex].lineColor:
            self.linecolor = QColor(mainWin.plotObject_1[selIndex].lineColor);
        else:
            print('Line color missing')
        self.edgecolor = QColor(mainWin.plotObject_1[selIndex].edgeColor);
        self.facecolor = QColor(mainWin.plotObject_1[selIndex].faceColor);
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
    def dafaultLineProp(self):
        linePropSelect = self.combo1.currentText()
        selIndexL = mainWin.YCols.indexFromItem(mainWin.YCols.findItems(linePropSelect,Qt.MatchFlag.MatchContains)[0])
        selIndex = int(selIndexL.row())
        mainWin.plotObject_1[selIndex] = PlotObject(label = linePropSelect, lineColor = mainWin.colorSet[selIndex])
        self.yLineProp()

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();
    def openFcColorDlg(self):
        self.facecolor = QColorDialog.getColor()
        self.pixmap.fill(self.facecolor);
        self.redIcon= QIcon(self.pixmap);
        self.fcBtn.setIcon(self.redIcon);
    def openEdColorDlg(self):
        self.edgecolor = QColorDialog.getColor()
        self.pixmap.fill(self.edgecolor);
        self.edCIcon= QIcon(self.pixmap);
        self.edBtn.setIcon(self.edCIcon);
    def openlnColorDlg(self):
        self.linecolor = QColorDialog.getColor()
        self.pixmap.fill(self.linecolor);
        self.lnCIcon= QIcon(self.pixmap);
        self.lnBtn.setIcon(self.lnCIcon);

class Header(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        self.frameD = QFrame(self);self.frameD.setGeometry(QRect(10, 25, 150, 25))
        self.def1 = QLabel("No of lines in Header:",self.frameD);
        self.frame2 = QFrame(self);self.frame2.setGeometry(QRect(200, 25, 75, 25))
        self.hBtn = QLineEdit("0",self.frame2);
        self.hBtn.setFixedWidth(75)
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(200, 70, 90, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)

    def okBtnAction(self):
        self.close();
        mainWin.header = int(self.hBtn.text())
        mainWin.readFile();

class MultiPlotPopup(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        x0 = 20
        y0 = 20

        wp = 10
        hp = 7
        yp1 = 5
        button_size = 25
#       y_pos = 20
        y_pos = y0
        y_button_size = 35

        self.frame2 = QFrame(self)
        self.Label1 = QLabel("single/multi:",self.frame2);
        myFont = QFont(); myFont.setBold(True); self.Label1.setFont(myFont)
        w1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).width()
        h1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).height()
        y_pos += yp1
        self.frame2.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frame4 = QFrame(self)
        self.Label3 = QLabel("multi-plot:",self.frame4)
        w1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).width()
        h1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).height()

        y_pos += + y0 + yp1
        self.frame4.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frameCB2 = QFrame(self);self.frameCB2.setGeometry(QRect(x0+w1+wp, y_pos, button_size, button_size))
        y_pos += h1 + hp

        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        x_button = 20
        y_button = y_pos + 10
        x_button_size = 90

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        x_button += x_button_size
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        x_button += x_button_size
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('black');

        self.x_end = x_button + x_button_size + 10
        self.y_end = y_button + y_button_size + 10

    def applyBtnAction(self):
        if mainWin.XIndex != 0:
            mainWin.displayMessage1("File Error", "x-variable must be time for multi-plot")
            return
        mainWin.multiPlotObject_1.multiPlot = self.cb2.isChecked()
        mainWin.plotDataWithChangedOptions()

    def MultiPlotPropShow(self):
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.multiPlotObject_1.multiPlot:
            bs = QtCore.Qt.CheckState.Checked
        self.cb2.setCheckState(bs)
    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class FourierPopup(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        x0 = 20
        y0 = 20

        wp = 10
        hp = 15
        yp1 = 5
        button_size = 25
        y_pos = y0
        y_button_size = 35

        self.frame1 = QFrame(self)
        self.Label1 = QLabel("Fourier:",self.frame1);
        myFont = QFont(); myFont.setBold(True); self.Label1.setFont(myFont)
        w1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).width()
        h1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).height()
        y_pos += yp1
        self.frame1.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frame2 = QFrame(self)
        self.Label3 = QLabel("Fourier:",self.frame2)
        w1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).width()
        h1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).height()

        y_pos += + y0 + yp1
        self.frame2.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frameCB2 = QFrame(self);self.frameCB2.setGeometry(QRect(x0+w1+wp, y_pos, button_size, button_size))
        y_pos += h1 + hp

        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame3 = QFrame(self)
        self.label_nfourier = QLabel("NFourier:",self.frame3)
        w1 = self.label_nfourier.fontMetrics().boundingRect(self.label_nfourier.text()).width()
        h1 = self.label_nfourier.fontMetrics().boundingRect(self.label_nfourier.text()).height()
        self.frame3.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        self.frame4 = QFrame(self)
        self.frame4.setGeometry(QRect(x0+w1+5, y_pos, 100, y_button_size))
        self.nFourierEdit = QLineEdit("10",self.frame4);
        self.nFourierEdit.setFixedWidth(50)
        y_pos += h1 + hp

        self.frame5 = QFrame(self)
        self.label_tstart = QLabel("tStart:",self.frame5)
        w1 = self.label_tstart.fontMetrics().boundingRect(self.label_tstart.text()).width()
        h1 = self.label_tstart.fontMetrics().boundingRect(self.label_tstart.text()).height()
        self.frame5.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        self.frame6 = QFrame(self)
        x_pos_1 = x0 + w1 + 5
        self.frame6.setGeometry(QRect(x_pos_1, y_pos, 200, y_button_size))
        self.tStartEdit = QLineEdit("0",self.frame6);
        self.tStartEdit.setFixedWidth(150)
        y_pos += h1 + hp

        self.frame7 = QFrame(self)
        self.label_tend = QLabel("tEnd:",self.frame7)
        w1 = self.label_tend.fontMetrics().boundingRect(self.label_tend.text()).width()
        h1 = self.label_tend.fontMetrics().boundingRect(self.label_tend.text()).height()
        self.frame7.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        self.frame8 = QFrame(self)
        self.frame8.setGeometry(QRect(x_pos_1, y_pos, 200, y_button_size))
        self.tEndEdit = QLineEdit("0",self.frame8);
        self.tEndEdit.setFixedWidth(150)
        y_pos += h1 + hp

        x_button = 20
        y_button = y_pos + 10
        x_button_size = 90

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        x_button += x_button_size
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        x_button += x_button_size
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('black');

        self.x_end = x_button + x_button_size + 10
        self.y_end = y_button + y_button_size + 10

    def applyBtnAction(self):
        print("fourier: apply button clicked")
        mainWin.fourierObject_1.fourier = self.cb2.isChecked()

        if mainWin.fourierObject_1.fourier:
            n_fourier = int(self.nFourierEdit.text())
            t_start = float(self.tStartEdit.text())
            t_end = float(self.tEndEdit.text())

            if t_start >= t_end:
                mainWin.displayMessage1("Time Error", "tStart >= tEnd?")
                return
            if mainWin.XIndex != 0:
                mainWin.displayMessage1("File Error", "x-variable must be time for Fourier analysis")
                return

            mainWin.fourierObject_1.setNFourier(n_fourier)
            mainWin.fourierObject_1.setTStart(t_start)
            mainWin.fourierObject_1.setTEnd(t_end)

            if mainWin.ext == '.csv':
                t = mainWin.reader.iloc[:, mainWin.XIndex]
            if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                t = mainWin.reader[:, mainWin.XIndex]

            if mainWin.ext == '.csv':
                mainWin.filename_fourier = mainWin.fileName.replace('.csv','_fourier.dat')
                mainWin.filename_thd = mainWin.fileName.replace('.csv','_thd.dat')
            elif mainWin.ext == '.txt':
                mainWin.filename_fourier = mainWin.fileName.replace('.txt','_fourier.dat')
                mainWin.filename_thd = mainWin.fileName.replace('.txt','_thd.dat')
            elif mainWin.ext == '.dat':
                mainWin.filename_fourier = mainWin.fileName.replace('.dat','_fourier.dat')
                mainWin.filename_thd = mainWin.fileName.replace('.dat','_thd.dat')

            n_x = len(mainWin.YIndex) + len(mainWin.YIndexR)
            thd = [0.0]*n_x
            x_fourier = []
            for i in range(n_x):
                l1 = [0.0]*n_fourier
                x_fourier.append(l1)

            n1 = 0
            for i in range(0,len(mainWin.YIndex)):
                if mainWin.ext == '.csv':
                    x1 = mainWin.reader.iloc[:,mainWin.YIndex[i]]
                if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                    x1 = mainWin.reader[:,mainWin.YIndex[i]]
                x_fourier[n1], thd[n1] = fourier_coeff(t, x1, t_start, t_end, n_fourier)
                n1 += 1
            for i in range(0,len(mainWin.YIndexR)):
                if mainWin.ext == '.csv':
                    x1 = mainWin.reader.iloc[:,mainWin.YIndexR[i]]
                if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                    x1 = mainWin.reader[:,mainWin.YIndexR[i]]
                x_fourier[n1], thd[n1] = fourier_coeff(t, x1, t_start, t_end, n_fourier)
                n1 += 1

            f_out = open(mainWin.filename_fourier, 'w')

            for i in range(n_fourier):
                s = "%3d"%(i)
                for j in range(n_x):
                    s += " %11.4E"%(x_fourier[j][i])
                f_out.write(s + "\n")
            f_out.close()

            f_out = open(mainWin.filename_thd, 'w')

            for j in range(n_x):
                s = " %9.3E"%(thd[j])
                f_out.write(s + "\n")
            f_out.close()

        mainWin.plotDataWithChangedOptions()
    def FourierPropShow(self):
        bs = QtCore.Qt.CheckState.Checked if mainWin.fourierObject_1.fourier else QtCore.Qt.CheckState.Unchecked
        self.cb2.setCheckState(bs)
    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class AvgRmsPopup(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        x0 = 20
        y0 = 20

        wp = 10
        hp = 15
        yp1 = 5
        button_size = 25
        y_pos = y0
        y_button_size = 35

        self.frame1 = QFrame(self)
        self.Label1 = QLabel("avg/rms:",self.frame1);
        myFont = QFont(); myFont.setBold(True); self.Label1.setFont(myFont)
        w1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).width()
        h1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).height()
        y_pos += yp1
        self.frame1.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frame2 = QFrame(self)
        self.Label2 = QLabel("avg:",self.frame2)
        w1 = self.Label2.fontMetrics().boundingRect(self.Label2.text()).width()
        h1 = self.Label2.fontMetrics().boundingRect(self.Label2.text()).height()

        y_pos += + y0 + yp1
        self.frame2.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frameCB2 = QFrame(self)
        x_pos_1 = x0+w1+wp
        self.frameCB2.setGeometry(QRect(x_pos_1, y_pos, button_size, button_size))

        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame3 = QFrame(self)
        self.Label3 = QLabel("rms:",self.frame3)
        w1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).width()
        h1 = self.Label3.fontMetrics().boundingRect(self.Label3.text()).height()

        y_pos += + y0 + yp1
        self.frame3.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frameCB3 = QFrame(self)
        self.frameCB3.setGeometry(QRect(x_pos_1, y_pos, button_size, button_size))

        self.cb3 = QCheckBox(self.frameCB3)
        self.cb3.setCheckState(QtCore.Qt.CheckState.Checked)

        y_pos += + h1 + yp1

        self.frame4 = QFrame(self)
        self.label_period = QLabel("period:",self.frame4)
        w1 = self.label_period.fontMetrics().boundingRect(self.label_period.text()).width()
        h1 = self.label_period.fontMetrics().boundingRect(self.label_period.text()).height()
        self.frame4.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        self.frame5 = QFrame(self)
        self.frame5.setGeometry(QRect(x0+w1+wp, y_pos, 200, y_button_size))
        self.periodEdit = QLineEdit("",self.frame5);
        self.periodEdit.setFixedWidth(150)
        y_pos += h1 + hp

        x_button = 20
        y_button = y_pos + 10
        x_button_size = 90

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        x_button += x_button_size
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        x_button += x_button_size
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('black');

        self.x_end = x_button + x_button_size + 10
        self.y_end = y_button + y_button_size + 10

    def applyBtnAction(self):
        print("avg/rms: apply button clicked")
        avg = self.cb2.isChecked()
        rms = self.cb3.isChecked()
        T = float(self.periodEdit.text())

        mainWin.avgrmsObject_1.setAvg(avg)
        mainWin.avgrmsObject_1.setRms(rms)
        mainWin.avgrmsObject_1.setPeriod(T)

        if mainWin.avgrmsObject_1.avg or mainWin.avgrmsObject_1.rms:
            if T <= 0.0:
                mainWin.displayMessage1("wrong period value", "period must be positive")
                return
            if mainWin.XIndex != 0:
                mainWin.displayMessage1("File Error", "x-variable must be time for avg/rms")
                return

            if mainWin.ext == '.csv':
                t = mainWin.reader.iloc[:, mainWin.XIndex]
            if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                t = mainWin.reader[:, mainWin.XIndex]

            x = []
            for i in range(0,len(mainWin.YIndex)):
                if mainWin.ext == '.csv':
                    x.append(mainWin.reader.iloc[:, mainWin.YIndex[i]])
                if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                    x.append(mainWin.reader[:, mainWin.YIndex[i]])

            for i in range(0,len(mainWin.YIndexR)):
                if mainWin.ext == '.csv':
                    x.append(mainWin.reader.iloc[:, mainWin.YIndexR[i]])
                if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                    x.append(mainWin.reader[:, mainWin.YIndexR[i]])

            n_x = len(mainWin.YIndex) + len(mainWin.YIndexR)
            if mainWin.avgrmsObject_1.avg:
                sum_avg = [0.0]*n_x
            if mainWin.avgrmsObject_1.rms:
                sum_rms = [0.0]*n_x

            x_last  = [0.0]*n_x
            x1      = [0.0]*n_x
            x1a     = [0.0]*n_x

            for j in range(n_x):
                x_last[j] = x[j][0]

            t_last = t[0]
            t0 = t[0]
            t_end = t0 + T 
            eps = 1.0e-3*T
            n  = len(t)
            flag_eps = False

            if mainWin.avgrmsObject_1.avg:
                x_avg = []
                for j in range(n_x):
                    x_avg.append([])
            if mainWin.avgrmsObject_1.rms:
                x_rms = []
                for j in range(n_x):
                    x_rms.append([])

            for i in range(1, n):
                t1 = t[i]
                for j in range(n_x):
                    x1[j] = x[j][i]

                if abs(t1-t_end) < eps:
                    if not flag_eps:
                        for j in range(n_x):
                            if mainWin.avgrmsObject_1.avg:
                                sum_avg[j] += 0.5*(x_last[j] + x1[j])*(t1 - t_last)
                                x_avg[j].append(sum_avg[j]/T)
                                sum_avg[j] = 0.0 
                            if mainWin.avgrmsObject_1.rms:
                                sum_rms[j] += 0.5*(x_last[j]*x_last[j] + x1[j]*x1[j])*(t1 - t_last)
                                x_rms[j].append(np.sqrt(sum_rms[j]/T))
                                sum_rms[j] = 0.0 
                            x_last[j] = x1[j]
                        t0 = t_end
                        t_end = t0 + T 
                        t_last = t1
                        flag_eps = True
                elif t1 > t_end:
                    for j in range(n_x):
                        x1a[j] = x_last[j] + ((x1[j]-x_last[j])/(t1-t_last))*(t_end-t_last)
                        if mainWin.avgrmsObject_1.avg:
                            sum_avg[j] += 0.5*(x_last[j] + x1a[j])*(t_end - t_last)
                            x_avg[j].append(sum_avg[j]/T)
                            sum_avg[j] = 0.0
                        if mainWin.avgrmsObject_1.rms:
                            sum_rms[j] += 0.5*(x_last[j]*x_last[j] + x1a[j]*x1a[j])*(t_end - t_last)
                            x_rms[j].append(np.sqrt(sum_rms[j]/T))
                            sum_rms[j] = 0.0
                        x_last[j] = x1a[j] 
                    t_last = t_end
                    t0 = t_end
                    t_end = t0 + T 
                    flag_eps = False
                else:
                    for j in range(n_x):
                        if mainWin.avgrmsObject_1.avg:
                            sum_avg[j] += 0.5*(x_last[j] + x1[j])*(t1 - t_last)
                        if mainWin.avgrmsObject_1.rms:
                            sum_rms[j] += 0.5*(x_last[j]*x_last[j] + x1[j]*x1[j])*(t1 - t_last)
                        x_last[j] = x1[j]
                    t_last = t1
                    flag_eps = False

            if mainWin.avgrmsObject_1.avg:
                mainWin.filename_avg = mainWin.fileName.replace('.dat','_avg.dat')
                f_out = open(mainWin.filename_avg, 'w')
                n_intervals = len(x_avg[0])
                for i in range(n_intervals):
                    t1_start = float(i)*T
                    t1_end = float(i+1)*T

                    s = "%11.4E"% (t1_start)
                    for j in range(n_x):
                        s += " %11.4E"%(x_avg[j][i])
                    f_out.write(s + "\n")

                    s = "%11.4E"% (t1_end)
                    for j in range(n_x):
                        s += " %11.4E"%(x_avg[j][i])
                    f_out.write(s + "\n")
                f_out.close()

            if mainWin.avgrmsObject_1.rms:
                mainWin.filename_rms = mainWin.fileName.replace('.dat','_rms.dat')
                f_out = open(mainWin.filename_rms, 'w')
                n_intervals = len(x_rms[0])
                for i in range(n_intervals):
                    t1_start = float(i)*T
                    t1_end = float(i+1)*T

                    s = "%11.4E"% (t1_start)
                    for j in range(n_x):
                        s += " %11.4E"%(x_rms[j][i])
                    f_out.write(s + "\n")

                    s = "%11.4E"% (t1_end)
                    for j in range(n_x):
                        s += " %11.4E"%(x_rms[j][i])
                    f_out.write(s + "\n")
                f_out.close()

        mainWin.plotDataWithChangedOptions()

    def avgRmsPropShow(self):
        bs = QtCore.Qt.CheckState.Checked if mainWin.avgrmsObject_1.avg else QtCore.Qt.CheckState.Unchecked
        self.cb2.setCheckState(bs)

        bs = QtCore.Qt.CheckState.Checked if mainWin.avgrmsObject_1.rms else QtCore.Qt.CheckState.Unchecked
        self.cb3.setCheckState(bs)

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class PowerPopup(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        x0 = 20
        y0 = 20

        wp = 10
        hp = 15
        yp1 = 5
        button_size = 25
        y_pos = y0
        y_button_size = 35

        y_pos += yp1
        self.frame1 = QFrame(self)
        self.Label1 = QLabel("Power Computation:",self.frame1);
        myFont = QFont(); myFont.setBold(True); self.Label1.setFont(myFont)
        w1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).width()
        h1 = self.Label1.fontMetrics().boundingRect(self.Label1.text()).height()
        self.frame1.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        y_pos += + y0 + yp1
        self.frame2 = QFrame(self)
        self.Label2 = QLabel("compute:",self.frame2)
        w1 = self.Label2.fontMetrics().boundingRect(self.Label2.text()).width()
        h1 = self.Label2.fontMetrics().boundingRect(self.Label2.text()).height()

        self.frame2.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frameCB2 = QFrame(self)
        self.frameCB2.setGeometry(QRect(x0+w1+wp, y_pos, button_size, button_size))

        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        y_pos += + y0 + yp1
        self.frame4 = QFrame(self)
        self.label_period = QLabel("period:",self.frame4)
        w1 = self.label_period.fontMetrics().boundingRect(self.label_period.text()).width()
        h1 = self.label_period.fontMetrics().boundingRect(self.label_period.text()).height()
        self.frame4.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        self.frame5 = QFrame(self)
        self.frame5.setGeometry(QRect(x0+w1+wp, y_pos, 200, y_button_size))
        self.periodEdit = QLineEdit("0.1",self.frame5);
        self.periodEdit.setFixedWidth(150)
        y_pos += h1 + hp

        self.frame6 = QFrame(self)
        self.label_voltage = QLabel("voltage:",self.frame6);
        myFont = QFont(); self.label_voltage.setFont(myFont)
        w1 = self.label_voltage.fontMetrics().boundingRect(self.label_voltage.text()).width()
        h1 = self.label_voltage.fontMetrics().boundingRect(self.label_voltage.text()).height()
        self.frame6.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))
        x_pos = x0+w1+wp

        w_combo = 100
        self.frame7 = QFrame(self)
        self.frame7.setGeometry(QRect(x_pos, y_pos, w_combo, h1+hp))
        self.combo_voltage = QComboBox(self.frame7)
        y_pos += h1 + hp

        self.frame8 = QFrame(self)
        self.label_voltage = QLabel("current:",self.frame8);
        myFont = QFont(); self.label_voltage.setFont(myFont)
        w1 = self.label_voltage.fontMetrics().boundingRect(self.label_voltage.text()).width()
        h1 = self.label_voltage.fontMetrics().boundingRect(self.label_voltage.text()).height()
        self.frame8.setGeometry(QRect(x0, y_pos, w1+wp, h1+hp))

        self.frame9 = QFrame(self)
        self.frame9.setGeometry(QRect(x_pos, y_pos, w_combo, h1+hp))
        self.combo_current = QComboBox(self.frame9)
        y_pos += h1 + hp

        x_button = 20
        y_button = y_pos + 10
        x_button_size = 90

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        x_button += x_button_size
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        x_button += x_button_size
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(x_button, y_button,
          x_button_size, y_button_size))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('black');

        self.x_end = x_button + x_button_size + 10
        self.y_end = y_button + y_button_size + 10

    def applyBtnAction(self):
        print("power: apply button clicked")
        compute_power = self.cb2.isChecked()
        T = float(self.periodEdit.text())

        mainWin.powerObject_1.setComputePower(compute_power)
        mainWin.powerObject_1.setPeriod(T)

        if mainWin.powerObject_1.compute_power:
            if T <= 0.0:
                mainWin.displayMessage1("wrong period value", "period must be positive")
                return
            v_string = self.combo_voltage.currentText()
            i_string = self.combo_current.currentText()
            print("PowerPopup: applyBtnAction: v_string:", v_string, "i_string:", i_string)
            mainWin.v_string = v_string
            mainWin.i_string = i_string
            mainWin.v_index = mainWin.fileVariables.index(v_string)
            mainWin.i_index = mainWin.fileVariables.index(i_string)
            print("PowerPopup: applyBtnAction: v_index:", mainWin.v_index, "i_index:", mainWin.i_index)

            if mainWin.ext == '.csv':
                t = mainWin.reader.iloc[:, 0]
                voltage = mainWin.reader.iloc[:, mainWin.v_index]
                current = mainWin.reader.iloc[:, mainWin.i_index]
            if (mainWin.ext == '.dat') | (mainWin.ext == '.txt'):
                t = mainWin.reader[:, 0]
                voltage = mainWin.reader[:, mainWin.v_index]
                current = mainWin.reader[:, mainWin.i_index]

            p = voltage*current

            sum_rms_v = 0.0
            sum_rms_i = 0.0
            sum_avg_p = 0.0

            x_last_v  = voltage[0]
            x_last_i  = current[0]
            x_last_p  = p[0]
            x1_v      = 0.0
            x1_i      = 0.0
            x1_p      = 0.0
            x1a_v     = 0.0
            x1a_i     = 0.0
            x1a_p     = 0.0

            t_last = t[0]
            t0 = t[0]
            t_end = t0 + T 
            eps = 1.0e-3*T
            n  = len(t)
            flag_eps = False

            x_rms_v = []
            x_rms_i = []
            x_avg_p = []

            for i in range(1, n):
                t1 = t[i]
                x1_v = voltage[i]
                x1_i = current[i]
                x1_p = p[i]

                if abs(t1-t_end) < eps:
                    if not flag_eps:
                        sum_rms_v += 0.5*(x_last_v*x_last_v + x1_v*x1_v)*(t1 - t_last)
                        x_rms_v.append(np.sqrt(sum_rms_v/T))
                        sum_rms_v = 0.0 
                        x_last_v = x1_v

                        sum_rms_i += 0.5*(x_last_i*x_last_i + x1_i*x1_i)*(t1 - t_last)
                        x_rms_i.append(np.sqrt(sum_rms_i/T))
                        sum_rms_i = 0.0 
                        x_last_i = x1_i

                        sum_avg_p += 0.5*(x_last_p + x1_p)*(t1 - t_last)
                        x_avg_p.append(sum_avg_p/T)
                        sum_avg_p = 0.0 
                        x_last_p = x1_p

                        t0 = t_end
                        t_end = t0 + T 
                        t_last = t1
                        flag_eps = True
                elif t1 > t_end:
                    x1a_v = x_last_v + ((x1_v-x_last_v)/(t1-t_last))*(t_end-t_last)
                    sum_rms_v += 0.5*(x_last_v*x_last_v + x1a_v*x1a_v)*(t_end - t_last)
                    x_rms_v.append(np.sqrt(sum_rms_v/T))
                    sum_rms_v = 0.0
                    x_last_v = x1a_v 

                    x1a_i = x_last_i + ((x1_i-x_last_i)/(t1-t_last))*(t_end-t_last)
                    sum_rms_i += 0.5*(x_last_i*x_last_i + x1a_i*x1a_i)*(t_end - t_last)
                    x_rms_i.append(np.sqrt(sum_rms_i/T))
                    sum_rms_i = 0.0
                    x_last_i = x1a_i 

                    x1a_p = x_last_p + ((x1_p-x_last_p)/(t1-t_last))*(t_end-t_last)
                    sum_avg_p += 0.5*(x_last_p + x1a_p)*(t_end - t_last)
                    x_avg_p.append(sum_avg_p/T)
                    sum_avg_p = 0.0
                    x_last_p = x1a_p 

                    t_last = t_end
                    t0 = t_end
                    t_end = t0 + T 
                    flag_eps = False
                else:
                    sum_rms_v += 0.5*(x_last_v*x_last_v + x1_v*x1_v)*(t1 - t_last)
                    x_last_v = x1_v

                    sum_rms_i += 0.5*(x_last_i*x_last_i + x1_i*x1_i)*(t1 - t_last)
                    x_last_i = x1_i

                    sum_avg_p += 0.5*(x_last_p + x1_p)*(t1 - t_last)
                    x_last_p = x1_p

                    t_last = t1
                    flag_eps = False

            mainWin.filename_power = mainWin.fileName.replace('.dat','_power.dat')
            f_out = open(mainWin.filename_power, 'w')
            n_intervals = len(x_rms_v)
            for i in range(n_intervals):
                t1_start = float(i)*T
                t1_end = float(i+1)*T
                p_app = x_rms_v[i]*x_rms_i[i]
                pf = x_avg_p[i]/p_app

                s = "%11.4E"% (t1_start)
                s1 = ''
                s1 += " %11.4E"%(x_rms_v[i])
                s1 += " %11.4E"%(x_rms_i[i])
                s1 += " %11.4E"%(x_avg_p[i])
                s1 += " %11.4E"%(p_app)
                s1 += " %11.4E"%(pf)
                f_out.write(s + s1 + "\n")

                s = "%11.4E"% (t1_end)
                f_out.write(s + s1 + "\n")
            f_out.close()

        mainWin.plotDataWithChangedOptions()

    def powerPropShow(self):
        bs = QtCore.Qt.CheckState.Checked if mainWin.powerObject_1.compute_power else QtCore.Qt.CheckState.Unchecked
        self.cb2.setCheckState(bs)

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class GridPopup(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        canvas = QPixmap(380, 130)
        canvas.fill(QColor("#FFFFFF"));
        myFont = QFont(); myFont.setBold(True);

        self.frameD = QFrame(self);self.frameD.setGeometry(QRect(10, 175, 50, 25))
        self.def1 = QLabel("Default:",self.frameD);self.def1.setFont(myFont)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(65, 175, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        self.cb1.stateChanged.connect(self.dafaultLineProp)

        self.frame2 = QFrame(self);self.frame2.setGeometry(QRect(20, 25, 250, 25))
        self.Label1 = QLabel("Grid:",self.frame2);
        myFont = QFont(); myFont.setBold(True);self.Label1.setFont(myFont)
        self.frame4 = QFrame(self);self.frame4.setGeometry(QRect(20, 50, 250, 25))
        self.Label3 = QLabel("Grid on-off:",self.frame4)
        self.frameCB2 = QFrame(self);self.frameCB2.setGeometry(QRect(90, 50, 25, 25))
        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame3 = QFrame(self);self.frame3.setGeometry(QRect(20, 85, 250, 25))
        self.Label2 = QLabel("Line Style:",self.frame3)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(90, 85, 250, 25))
        self.combo2 = QComboBox(self.frame7)
        self.combo2.addItem("solid");self.combo2.addItem("None");self.combo2.addItem("dotted")
        self.combo2.addItem("dashed");self.combo2.addItem("dashdot");

        self.frame6 = QFrame(self);self.frame6.setGeometry(QRect(20, 120, 250, 25))
        self.Label5 = QLabel("Color:",self.frame6)
        self.frame25 = QFrame(self);self.frame25.setGeometry(QRect(100,120,30,25))
        self.lnBtn = QPushButton(self.frame25);self.lnBtn.clicked.connect(self.openlnColorDlg)
        self.pixmap = QPixmap(10,10);self.pixmap.fill(QColor("red"));
        self.lnCIcon= QIcon(self.pixmap);self.lnBtn.setIcon(self.lnCIcon);

        self.frame12 = QFrame(self);self.frame12.setGeometry(QRect(220, 50, 250, 25))
        self.Label7 = QLabel("Which:",self.frame12)
        self.frame13 = QFrame(self);self.frame13.setGeometry(QRect(300, 50, 250, 25))
        self.combo4 = QComboBox(self.frame13)
        self.combo4.addItem("major");self.combo4.addItem("minor");self.combo4.addItem("both");

        self.frame14 = QFrame(self);self.frame14.setGeometry(QRect(220, 85, 250, 25))
        self.Label8 = QLabel("Width:",self.frame14)
        self.frame27 = QFrame(self);self.frame27.setGeometry(QRect(300,85,115,25))
        self.sizeBtn = QLineEdit("0.5",self.frame27);
        self.sizeBtn.setFixedWidth(75)
        validator = QDoubleValidator()
        self.sizeBtn.setValidator(validator)

        self.frame16 = QFrame(self);self.frame16.setGeometry(QRect(220, 120, 250, 25))
        self.Label9 = QLabel("Axis:",self.frame16)
        self.frame24 = QFrame(self);self.frame24.setGeometry(QRect(300,120,150,25))
        self.combo3 = QComboBox(self.frame24)
        self.combo3.addItem("both");self.combo3.addItem("x");self.combo3.addItem("y");

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(120, 170, 90, 25))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(210, 170, 90, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(300, 170, 90, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('black');

    def applyBtnAction(self):
        mainWin.gridObject_1.setLineStyle(self.combo2.currentText())
        mainWin.gridObject_1.setAxis(self.combo3.currentText())
        mainWin.gridObject_1.setWidth(float(self.sizeBtn.text()))
        mainWin.gridObject_1.setWhich(self.combo4.currentText())
        mainWin.gridObject_1.setLineColor(self.linecolor.name())
        mainWin.gridObject_1.gridEnable = self.cb2.isChecked()
        mainWin.plotCanvas_1.changeGridProps()
        mainWin.plotWindow_1.canvas.changeGridProps()
    def GridPropShow(self):
        self.combo2.setCurrentText(mainWin.gridObject_1.lineStyle)
        self.combo3.setCurrentText(mainWin.gridObject_1.axis)
        self.sizeBtn.setText(str(mainWin.gridObject_1.width))

        self.pixmap.fill(QColor(mainWin.gridObject_1.lineColor));
        self.redIcon= QIcon(self.pixmap);
        self.lnBtn.setIcon(self.redIcon);

        self.combo4.setCurrentText(mainWin.gridObject_1.which)
        self.linecolor = QColor(mainWin.gridObject_1.lineColor);
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.gridObject_1.gridEnable:
            bs = QtCore.Qt.CheckState.Checked
        self.cb2.setCheckState(bs)
    def dafaultLineProp(self):
        mainWin.gridObject_1 = GridObject()
        self.GridPropShow()

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

    def openlnColorDlg(self):
        self.linecolor = QColorDialog.getColor()
        self.pixmap.fill(self.linecolor);
        self.lnCIcon= QIcon(self.pixmap);
        self.lnBtn.setIcon(self.lnCIcon);

class AxesPopup(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        canvas = QPixmap(185, 130)
        canvas.fill(QColor("#FFFFFF"));
        myFont = QFont(); myFont.setBold(True);

        self.frameD = QFrame(self);self.frameD.setGeometry(QRect(10, 145, 50, 25))
        self.def1 = QLabel("Default:",self.frameD);self.def1.setFont(myFont)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(65, 145, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        self.cb1.stateChanged.connect(self.dafaultLineProp)

        self.frame2 = QFrame(self);self.frame2.setGeometry(QRect(20, 15, 250, 25))
        self.Label1 = QLabel("X-Axis:",self.frame2);self.Label1.setFont(myFont)
        self.frame3 = QFrame(self);self.frame3.setGeometry(QRect(20, 35, 250, 25))
        self.Label2 = QLabel("Scale:",self.frame3)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(60, 35, 250, 25))
        self.combo2 = QComboBox(self.frame7)
        self.combo2.addItem("linear");self.combo2.addItem("log")

        self.frame4 = QFrame(self);self.frame4.setGeometry(QRect(20, 60, 250, 25))
        self.Label3 = QLabel("Label:",self.frame4)
        self.frame8 = QFrame(self);self.frame8.setGeometry(QRect(60, 60, 250, 25))
        self.labelEdit = QLineEdit("X-Axis",self.frame8);
        self.labelEdit.setFixedWidth(120)

        self.frame5 = QFrame(self);self.frame5.setGeometry(QRect(20, 85, 250, 25))
        self.Label4 = QLabel("Left:",self.frame5)
        self.frame6 = QFrame(self);self.frame6.setGeometry(QRect(60, 85, 250, 25))
        self.XLimL = QLineEdit("",self.frame6);
        self.XLimL.setFixedWidth(120)
        validator = QDoubleValidator()
        self.XLimL.setValidator(validator)

        self.frame6 = QFrame(self);self.frame6.setGeometry(QRect(20, 110, 250, 25))
        self.Label5 = QLabel("Right:",self.frame6)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(60, 110, 250, 25))
        self.XLimR = QLineEdit("",self.frame7);
        self.XLimR.setFixedWidth(120)
        validator = QDoubleValidator()
        self.XLimR.setValidator(validator)

        self.frame11 = QFrame(self);self.frame11.setGeometry(QRect(220, 15, 250, 25))
        self.Label6 = QLabel("Y-Axis:",self.frame11);self.Label6.setFont(myFont)

        self.frame12 = QFrame(self);self.frame12.setGeometry(QRect(220, 35, 250, 25))
        self.Label7 = QLabel("Scale:",self.frame12)
        self.frame13 = QFrame(self);self.frame13.setGeometry(QRect(270, 35, 250, 25))
        self.combo4 = QComboBox(self.frame13)
        self.combo4.addItem("linear");self.combo4.addItem("log")

        self.frame14 = QFrame(self);self.frame14.setGeometry(QRect(220, 60, 250, 25))
        self.Label8 = QLabel("Label:",self.frame14)
        self.frame15 = QFrame(self);self.frame15.setGeometry(QRect(270, 60, 250, 25))
        self.YlabelEdit = QLineEdit("Y-Axis",self.frame15);
        self.YlabelEdit.setFixedWidth(120)

        self.frame16 = QFrame(self);self.frame16.setGeometry(QRect(220, 85, 250, 25))
        self.Label9 = QLabel("Bottom:",self.frame16)
        self.frame24 = QFrame(self);self.frame24.setGeometry(QRect(270,85,250,25))
        self.YlimB = QLineEdit("0",self.frame24);
        self.YlimB.setFixedWidth(120)
        validator = QDoubleValidator()
        self.YlimB.setValidator(validator)

        self.frame17 = QFrame(self);self.frame17.setGeometry(QRect(220, 110, 250, 25))
        self.Label10 = QLabel("Top:",self.frame17)
        self.frame23 = QFrame(self);self.frame23.setGeometry(QRect(270,110,250,25))
        self.YlimT = QLineEdit("10",self.frame23);
        self.YlimT.setFixedWidth(120)
        validator = QDoubleValidator()
        self.YlimT.setValidator(validator)

        self.frameY11 = QFrame(self);self.frameY11.setGeometry(QRect(420, 15, 250, 25))
        self.LabelY6 = QLabel("Y-Axis (Right):",self.frameY11);self.LabelY6.setFont(myFont)

        self.frameY12 = QFrame(self);self.frameY12.setGeometry(QRect(420, 35, 250, 25))
        self.LabelY7 = QLabel("Scale:",self.frameY12)
        self.frameY13 = QFrame(self);self.frameY13.setGeometry(QRect(470, 35, 250, 25))
        self.comboY4 = QComboBox(self.frameY13)
        self.comboY4.addItem("linear");self.comboY4.addItem("log")

        self.frameY14 = QFrame(self);self.frameY14.setGeometry(QRect(420, 60, 250, 25))
        self.LabelY8 = QLabel("Label:",self.frameY14)
        self.frameY15 = QFrame(self);self.frameY15.setGeometry(QRect(470, 60, 250, 25))
        self.YlabelEdit2 = QLineEdit("Y-Axis",self.frameY15);
        self.YlabelEdit2.setFixedWidth(120)

        self.frameY16 = QFrame(self);self.frameY16.setGeometry(QRect(420, 85, 250, 25))
        self.LabelY9 = QLabel("Bottom:",self.frameY16)
        self.frameY24 = QFrame(self);self.frameY24.setGeometry(QRect(470,85,250,25))
        self.YlimB2 = QLineEdit("0",self.frameY24);
        self.YlimB2.setFixedWidth(120)
        validator = QDoubleValidator()
        self.YlimB2.setValidator(validator)

        self.frameY17 = QFrame(self);self.frameY17.setGeometry(QRect(420, 110, 250, 25))
        self.LabelY10 = QLabel("Top:",self.frameY17)
        self.frameY23 = QFrame(self);self.frameY23.setGeometry(QRect(470,110,250,25))
        self.YlimT2 = QLineEdit("10",self.frameY23);
        self.YlimT2.setFixedWidth(120)
        validator = QDoubleValidator()
        self.YlimT2.setValidator(validator)

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(120, 140, 90, 25))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(210, 140, 90, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(300, 140, 90, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)

    def applyBtnAction(self):
        if self.combo2.currentText()!='None':
            mainWin.axesPropObject_1.setxScale(self.combo2.currentText())
        if self.labelEdit.text() !='None':
            mainWin.axesPropObject_1.setxLabel(self.labelEdit.text())

        print('AxesPopup: applyBtnAction: setting xmin')
        if self.XLimL.text().strip() != '':
            mainWin.axesPropObject_1.setxMin(self.XLimL.text())
        if self.XLimR.text().strip() !='':
            mainWin.axesPropObject_1.setxMax(self.XLimR.text())
        print('AxesPopup: applyBtnAction: mainWin.axesPropObject_1.s_xMin:', \
            mainWin.axesPropObject_1.s_xMin)

        if self.YlimB.text().strip() != '':
            mainWin.axesPropObject_1.setyMin(self.YlimB.text())
        if self.YlimT.text().strip() != '':
            mainWin.axesPropObject_1.setyMax(self.YlimT.text())

        if self.YlimB2.text().strip() != '':
            mainWin.axesPropObject_1.setyMin2(self.YlimB2.text())
        if self.YlimT2.text().strip() != '':
            mainWin.axesPropObject_1.setyMax2(self.YlimT2.text())

        mainWin.axesPropObject_1.setyScale(self.combo4.currentText())
        mainWin.axesPropObject_1.setyLabel(self.YlabelEdit.text())
        mainWin.axesPropObject_1.setyScale2(self.comboY4.currentText())
        mainWin.axesPropObject_1.setyLabel2(self.YlabelEdit2.text())

        if not (mainWin.powerObject_1.compute_power or \
           mainWin.multiPlotObject_1.multiPlot or \
           mainWin.fourierObject_1.fourier):
            mainWin.plotCanvas_1.changeAxesProps()
            mainWin.plotWindow_1.canvas.changeAxesProps()
        else:
            mainWin.plotDataWithChangedOptions()

    def AxesPropShow(self):
        self.combo2.setCurrentText(mainWin.axesPropObject_1.xScale)
        self.combo4.setCurrentText(mainWin.axesPropObject_1.yScale)
        self.comboY4.setCurrentText(mainWin.axesPropObject_1.yScale2)

        self.labelEdit.setText(mainWin.axesPropObject_1.xLabel)
        self.YlabelEdit.setText(mainWin.axesPropObject_1.yLabel)
        self.YlabelEdit2.setText(mainWin.axesPropObject_1.yLabel2)

        self.XLimL.setText(mainWin.axesPropObject_1.s_xMin)
        self.XLimR.setText(mainWin.axesPropObject_1.s_xMax)

        self.YlimB.setText(mainWin.axesPropObject_1.s_yMin)
        self.YlimT.setText(mainWin.axesPropObject_1.s_yMax)

        self.YlimB2.setText(mainWin.axesPropObject_1.s_yMin2)
        self.YlimT2.setText(mainWin.axesPropObject_1.s_yMax2)

        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
    def dafaultLineProp(self):
        mainWin.axesPropObject_1 = AxesPropObject()
        self.AxesPropShow()

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class TitlePopup(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        canvas = QPixmap(250, 100)
        canvas.fill(QColor("#FFFFFF"));
        myFont = QFont(); myFont.setBold(True);

        self.frameD = QFrame(self);self.frameD.setGeometry(QRect(10, 145, 50, 25))
        self.def1 = QLabel("Default:",self.frameD);self.def1.setFont(myFont)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(65, 145, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        self.cb1.stateChanged.connect(self.dafaultLineProp)

        self.frame2 = QFrame(self);self.frame2.setGeometry(QRect(20, 15, 250, 25))
        self.Label1 = QLabel("Title:",self.frame2);self.Label1.setFont(myFont)

        self.frameT = QFrame(self);self.frameT.setGeometry(QRect(20, 30, 50, 25))
        self.def1 = QLabel("Title:",self.frameT);
        self.frameTE = QFrame(self);self.frameTE.setGeometry(QRect(80, 30, 25, 25))
        self.cb3 = QCheckBox(self.frameTE)
        self.cb3.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame3 = QFrame(self);self.frame3.setGeometry(QRect(20, 55, 250, 25))
        self.Label2 = QLabel("Position:",self.frame3)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(80, 55, 250, 25))
        self.combo2 = QComboBox(self.frame7)
        self.combo2.addItem("left");self.combo2.addItem("center");self.combo2.addItem("right")

        self.frame4 = QFrame(self);self.frame4.setGeometry(QRect(20, 80, 250, 25))
        self.Label3 = QLabel("Label:",self.frame4)
        self.frame8 = QFrame(self);self.frame8.setGeometry(QRect(80, 80, 250, 25))
        self.labelEdit = QLineEdit("",self.frame8);
        self.labelEdit.setFixedWidth(175)

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(20, 170, 80, 25))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)

        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(120, 170,80, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)

        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(220, 170, 80, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)

    def applyBtnAction(self):
        mainWin.titleObject_1.setLoc(self.combo2.currentText())
        mainWin.titleObject_1.setLabel(self.labelEdit.text())
        mainWin.titleObject_1.setTitleEnable(self.cb3.isChecked())
        mainWin.plotCanvas_1.changeTitle()
        mainWin.plotWindow_1.canvas.changeTitle()
    def TitlePropShow(self):
        self.combo2.setCurrentText(str(mainWin.titleObject_1.loc))
        self.labelEdit.setText(str(mainWin.titleObject_1.label))
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.titleObject_1.titleEnable:
            bs = QtCore.Qt.CheckState.Checked
        self.cb3.setCheckState(bs)

    def dafaultLineProp(self):
        mainWin.titleObject_1 = TitleObject()
        self.cb1.setCheckState(QtCore.Qt.CheckState.Unchecked)
        self.TitlePropShow()

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();
class TickLabelNotation(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();
    def widget(self):
        canvas = QPixmap(460, 130)
        canvas.fill(QColor("#FFFFFF"));
        myFont = QFont(); myFont.setBold(False);

        self.frameEg = QFrame(self);self.frameEg.setGeometry(QRect(20, 15, 450, 40))
        self.LabelEg = QLabel(
            "example: m=-3, n=3 ticklabels beyond 10\u207b\u00b3 - 10\u00b3 will be converted to" + \
             "\n       scientific notation",self.frameEg)
        self.LabelEg.setFont(myFont)

        self.frameSN = QFrame(self);self.frameSN.setGeometry(QRect(20, 50, 250, 25))
        self.LabelSN = QLabel("Limits For Scientific Notation:",self.frameSN);self.LabelSN.setFont(myFont)
        self.frameSNLb = QFrame(self);self.frameSNLb.setGeometry(QRect(20, 75, 170, 25))
        self.SNLb = QLabel("X-TickLabels: m:",self.frameSNLb);self.SNLb.setFont(myFont)
        self.frameSNL = QFrame(self);self.frameSNL.setGeometry(QRect(125, 75, 50, 25))
        self.SNLEdit = QLineEdit("-3",self.frameSNL)
        self.SNLEdit.setFixedWidth(40)
        self.frameSNLu = QFrame(self);self.frameSNLu.setGeometry(QRect(180, 75, 150, 25))
        self.SNLu = QLabel("n:",self.frameSNLu);self.SNLu.setFont(myFont)
        self.frameSNU = QFrame(self);self.frameSNU.setGeometry(QRect(205, 75, 50, 25))
        self.SNUEdit = QLineEdit("3",self.frameSNU)
        self.SNUEdit.setFixedWidth(40)
        self.frameSNXM = QFrame(self);self.frameSNXM.setGeometry(QRect(260, 75, 150, 25))
        self.SNXM = QLabel("UseMathsText:",self.frameSNXM);self.SNXM.setFont(myFont)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(355, 75, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frameYSNLb = QFrame(self);self.frameYSNLb.setGeometry(QRect(20, 100, 170, 25))
        self.YSNLb = QLabel("Y-TickLabels: m:",self.frameYSNLb);self.YSNLb.setFont(myFont)
        self.frameYSNL = QFrame(self);self.frameYSNL.setGeometry(QRect(125, 100, 50, 25))
        self.YSNLEdit = QLineEdit("-3",self.frameYSNL)
        self.YSNLEdit.setFixedWidth(40)
        self.frameYSNLu = QFrame(self);self.frameYSNLu.setGeometry(QRect(180, 100, 150, 25))
        self.YSNLu = QLabel("n:",self.frameYSNLu);self.YSNLu.setFont(myFont)
        self.frameYSNU = QFrame(self);self.frameYSNU.setGeometry(QRect(205, 100, 50, 25))
        self.YSNUEdit = QLineEdit("3",self.frameYSNU)
        self.YSNUEdit.setFixedWidth(40)
        self.frameSNYM = QFrame(self);self.frameSNYM.setGeometry(QRect(260, 100, 150, 25))
        self.SNYM = QLabel("UseMathsText:",self.frameSNYM);self.SNYM.setFont(myFont)
        self.frameCB2 = QFrame(self);self.frameCB2.setGeometry(QRect(355, 100, 25, 25))
        self.cb2 = QCheckBox(self.frameCB2)
        self.cb2.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame23 =  QFrame(self);self.frame23.setGeometry(QRect(20, 170, 80, 25))
        self.applyBtn = QPushButton("Apply",self.frame23)
        self.applyBtn.clicked.connect(self.applyBtnAction)
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(120, 170,80, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(220, 170, 80, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();
    def applyBtnAction(self):
        if self.YSNUEdit.text() != 'None':
            mainWin.axesPropObject_1.setySNU(float(self.YSNUEdit.text()))
        if self.YSNLEdit.text() != 'None':
            mainWin.axesPropObject_1.setySNL(float(self.YSNLEdit.text()))
        if self.SNUEdit.text() != 'None':
            mainWin.axesPropObject_1.setxSNU(float(self.SNUEdit.text()))
        if self.SNLEdit.text() != 'None':
            mainWin.axesPropObject_1.setxSNL(float(self.SNLEdit.text()))
        bs = QtCore.Qt.CheckState.Unchecked
        if self.cb2.isChecked():
            bs = QtCore.Qt.CheckState.Checked
            mainWin.axesPropObject_1.setySNM(True)
        else:
            mainWin.axesPropObject_1.setySNM(False)

        if self.cb1.isChecked():
            bs = QtCore.Qt.CheckState.Checked
            mainWin.axesPropObject_1.setxSNM(True)
        else:
            mainWin.axesPropObject_1.setxSNM(False)
        mainWin.plotCanvas_1.changeAxesProps()
        mainWin.plotWindow_1.canvas.changeAxesProps()

class LegendPopup(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self)
        self.widget();

    def widget(self):
        canvas = QPixmap(380, 125)
        canvas.fill(QColor("#FFFFFF"));

        self.frame1 = QFrame(self);self.frame1.setGeometry(QRect(20, 10, 250, 25))
        self.combo1 = QLabel("Show Legend",self.frame1)
        self.frameCB1 = QFrame(self);self.frameCB1.setGeometry(QRect(150, 10, 25, 25))
        self.cb1 = QCheckBox(self.frameCB1)
        self.cb1.setCheckState(QtCore.Qt.CheckState.Checked)

        myFont = QFont(); myFont.setBold(True);
        self.frame3 = QFrame(self);self.frame3.setGeometry(QRect(20, 40, 250, 25))
        self.Label2 = QLabel("Location:",self.frame3)
        self.frame7 = QFrame(self);self.frame7.setGeometry(QRect(90, 40, 250, 25))
        self.combo2 = QComboBox(self.frame7)
        self.combo2.addItem("best");self.combo2.addItem("upper right");self.combo2.addItem("upper left")
        self.combo2.addItem("lower left");self.combo2.addItem("lower right");
        self.combo2.addItem("lower center");self.combo2.addItem("upper center");
        self.combo2.addItem("center left");self.combo2.addItem("center right");self.combo2.addItem("center");

        self.frame4 = QFrame(self);self.frame4.setGeometry(QRect(20, 70, 250, 25))
        self.Label3 = QLabel("Font Size:",self.frame4)
        self.frame8 = QFrame(self);self.frame8.setGeometry(QRect(90, 70, 250, 25))
        self.fSize = QLineEdit("10",self.frame8);
        self.fSize.setFixedWidth(75)
        validator = QDoubleValidator()
        self.fSize.setValidator(validator)
        self.frame5 = QFrame(self);self.frame5.setGeometry(QRect(20, 100, 250, 25))
        self.Label4 = QLabel("Title:",self.frame5)
        self.frame26 = QFrame(self);self.frame26.setGeometry(QRect(90,100,115,25))
        self.legTitle = QLineEdit(None,self.frame26);
        self.legTitle.setFixedWidth(95)
        self.frame6 = QFrame(self);self.frame6.setGeometry(QRect(20, 130, 250, 25))
        self.Label5 = QLabel("Frame:",self.frame6)
        self.frame25 = QFrame(self);self.frame25.setGeometry(QRect(100,130,30,25))
        self.fr1 = QCheckBox(self.frame25)
        self.fr1.setCheckState(QtCore.Qt.CheckState.Checked)

        self.frame12 = QFrame(self);self.frame12.setGeometry(QRect(205, 40, 250, 25))
        self.Label7 = QLabel("Label Spacing:",self.frame12)
        self.frame13 = QFrame(self);self.frame13.setGeometry(QRect(308, 40, 250, 25))
        self.lblSpc = QLineEdit("0.5",self.frame13);
        self.lblSpc.setFixedWidth(75)
        validator = QDoubleValidator()
        self.lblSpc.setValidator(validator)
        self.frame14 = QFrame(self);self.frame14.setGeometry(QRect(205, 70, 250, 25))
        self.Label8 = QLabel("Marker Scale:",self.frame14)
        self.frame27 = QFrame(self);self.frame27.setGeometry(QRect(308,70,115,25))
        self.mscale = QLineEdit("1.0",self.frame27);
        self.mscale.setFixedWidth(75)
        validator = QDoubleValidator()
        self.mscale.setValidator(validator)
        self.frame16 = QFrame(self);self.frame16.setGeometry(QRect(205, 100, 250, 25))
        self.Label9 = QLabel("Marker First:",self.frame16)
        self.frame24 = QFrame(self);self.frame24.setGeometry(QRect(308,100,30,25))
        self.m1 = QCheckBox(self.frame24)
        self.m1.setCheckState(QtCore.Qt.CheckState.Checked)
        self.frame17 = QFrame(self);self.frame17.setGeometry(QRect(205, 130, 250, 25))
        self.Label10 = QLabel("Column Spacing:",self.frame17)
        self.frame23 = QFrame(self);self.frame23.setGeometry(QRect(308,125,250,25))
        self.clmSpc = QLineEdit("2.0",self.frame23);
        self.clmSpc.setFixedWidth(75)
        validator = QDoubleValidator()
        self.clmSpc.setValidator(validator)

        self.frame20 =  QFrame(self);self.frame20.setGeometry(QRect(120, 170, 90, 25))
        self.applyBtn = QPushButton("Apply",self.frame20);
        self.applyBtn.clicked.connect(self.applyBtnAction)
        self.frame21 =  QFrame(self);self.frame21.setGeometry(QRect(210, 170, 90, 25))
        self.cancelBtn = QPushButton("Cancel",self.frame21)
        self.cancelBtn.clicked.connect(self.cancelBtnAction)
        self.frame22 =  QFrame(self);self.frame22.setGeometry(QRect(300, 170, 90, 25))
        self.okBtn = QPushButton("Ok",self.frame22)
        self.okBtn.clicked.connect(self.okBtnAction)
        self.linecolor = QColor('red');
        self.edgecolor = QColor('black');
        self.facecolor = QColor('red');

    def applyBtnAction(self):
        mainWin.legendObject_1.setlocation(self.combo2.currentText())
        mainWin.legendObject_1.setfontsize(float(self.fSize.text()))
        mainWin.legendObject_1.settitle((self.legTitle.text()))
        mainWin.legendObject_1.setlabelspacing(float(self.lblSpc.text()))
        mainWin.legendObject_1.setmarkerscale(float(self.mscale.text()))
        mainWin.legendObject_1.setcolumnspacing(float(self.clmSpc.text()))
        mainWin.legendObject_1.setmarkerfirst(self.m1.isChecked())
        mainWin.legendObject_1.setframeon(self.fr1.isChecked())
        mainWin.legendEnable = self.cb1.isChecked()
        if mainWin.legendEnable:
            mainWin.plotCanvas_1.showLegend();
            mainWin.plotWindow_1.canvas.showLegend();
        else:
            mainWin.plotCanvas_1.removeLegend()
            mainWin.plotWindow_1.canvas.removeLegend()

    def LegendPropShow(self):
        self.combo2.setCurrentText(mainWin.legendObject_1.location)
        self.fSize.setText(str(mainWin.legendObject_1.fontsize))
        self.legTitle.setText(mainWin.legendObject_1.title)
        self.lblSpc.setText(str(mainWin.legendObject_1.labelspacing))
        self.mscale.setText(str(mainWin.legendObject_1.markerscale))
        self.clmSpc.setText(str(mainWin.legendObject_1.columnspacing))
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.legendObject_1.markerfirst :
            bs = QtCore.Qt.CheckState.Checked
        self.m1.setCheckState(bs)
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.legendObject_1.frameon:
            bs = QtCore.Qt.CheckState.Checked
        self.fr1.setCheckState(bs)
        bs = QtCore.Qt.CheckState.Unchecked
        if mainWin.legendEnable:
            bs = QtCore.Qt.CheckState.Checked
        self.cb1.setCheckState(bs)

    def cancelBtnAction(self):
        self.close();
    def okBtnAction(self):
        self.close();

class ApplicationWindow(QMainWindow):

    n_col = 0; total_rows=0;

    def __init__(self, dir_output_files, file_last_opened):
        super().__init__()

        self.dir_output_files = dir_output_files
        self.file_last_opened = file_last_opened

        self.title = "Demo"
        self.left = 200
        self.top = 50
        self.width = 1000
        self.height = 620
        self.resize(self.width, self.height)
        self.move(self.left, self.top)

        self.n_variables = 0
        self.n_variables_max = 3

        self.v_string = ''
        self.i_string = ''
        self.v_index = 0
        self.i_index = 0

        self.filename_fourier = ''
        self.filename_thd = ''

        self.widgetR();

    def widgetR(self):
        self.colorSet = ["dimgrey", "royalblue", "green", "tomato", "mediumorchid",
           "crimson", "lightseagreen", "peru", "palevioletred"]
        self.nColorSet = len(self.colorSet)
        self._main = QtWidgets.QWidget()
        layout = QtWidgets.QVBoxLayout()#._main)
        self.DefaultPath = ""
        self.msg = QMessageBox()
        self.frame1 = QFrame(self)
        self.frame1.setGeometry(QRect(10, 40, 100, 25))
        self.LegBtn1 = QPushButton("Load File",self.frame1)
        self.LegBtn1.setChecked(True)
        self.LegBtn1.clicked.connect(self.openFileNameDialog)
        layout.addWidget(self.LegBtn1)

        self.frame6 = QFrame(self)
        self.frame6.setGeometry(QRect(300, 570, 1000, 30))
        self.labelFile = QLabel("Placeholder for file path and file name ",self.frame6)

        self.frame2 = QFrame(self)
        self.frame2.setGeometry(QRect(10, 237, 350, 30))
        self.label = QLabel("Number of columns:     ",self.frame2)
        layout.addWidget(self.label)
        self.label.setEnabled(0)

        self.frame5 = QFrame(self)
        self.frame5.setGeometry(QRect(10, 255, 350, 30))
        self.labelR = QLabel("Number of rows:                ",self.frame5)
        layout.addWidget(self.labelR)
        self.labelR.setEnabled(0)

        self.framegf = QFrame(self)
        self.framegf.setGeometry(QRect(300, 555, 100, 30))
        self.Labelf = QLabel("File Path:",self.framegf)
        layout.addWidget(self.Labelf)

        layout.addWidget(self.labelFile)
        self.labelFile.setEnabled(0)
        self.scrollFile = QScrollArea(self)
        self.scrollFile.setWidget(self.labelFile)
        self.scrollFile.setWidgetResizable(True)
        self.scrollFile.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarPolicy.ScrollBarAlwaysOff);
        self.scrollFile.setFixedHeight(45)
        self.scrollFile.setFixedWidth(680)
        layout.addWidget(self.scrollFile)
        self.scrollFile.move(300,570)

        self.framefn = QFrame(self)
        self.framefn.setGeometry(QRect(10, 65, 272, 30))
        self.labelFilen = QLabel("Placeholder for file name ",self.framefn)
        self.labelFilen.setEnabled(0)
        layout.addWidget(self.labelFile)
        self.scrollFilen = QScrollArea(self)
        self.scrollFilen.setWidget(self.labelFilen)
        self.scrollFilen.setWidgetResizable(True)
        self.scrollFilen.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarPolicy.ScrollBarAlwaysOff);
        self.scrollFilen.setFixedHeight(45)
        self.scrollFilen.setFixedWidth(172)
        self.scrollFilen.setFrameShape(QFrame.Shape.NoFrame)
        layout.addWidget(self.scrollFilen)
        self.scrollFilen.move(10,65)

        self.frameg8 = QFrame(self)
        self.frameg8.setGeometry(QRect(10, 117, 250, 30))
        self.Labelg = QLabel("Output Files:",self.frameg8)
        layout.addWidget(self.Labelg)

        self.frameg9 = QFrame(self)
        self.frameg9.setGeometry(QRect(10, 133, 200, 150))
        self.OPFiles = QListWidget(self.frameg9)
        self.OPFiles.clicked.connect(self.FileSelectionChange)
        self.OPFiles.setSelectionMode(QAbstractItemView.SelectionMode.NoSelection)
        layout.addWidget(self.OPFiles)
        self.scrollg2 = QScrollArea(self)
        self.scrollg2.setWidget(self.OPFiles)
        self.scrollg2.setWidgetResizable(True)
        self.scrollg2.setFixedHeight(100)
        self.scrollg2.setFixedWidth(150)
        layout.addWidget(self.scrollg2)
        self.scrollg2.move(10,133)
        self.FileIndex = None;

        self.frame3 = QFrame(self)
        self.frame3.setGeometry(QRect(10, 272, 250, 30))
        self.LabelX = QLabel("X-axis:",self.frame3)
        layout.addWidget(self.LabelX)
        self.frame7 = QFrame(self)
        self.frame7.setGeometry(QRect(10, 290, 200, 200))
        self.senList = QListWidget(self.frame7)
        self.senList.clicked.connect(self.XSelectionChange)

        self.senList.setSelectionMode(QAbstractItemView.SelectionMode.NoSelection)
        layout.addWidget(self.senList)
        self.scroll = QScrollArea(self)
        self.scroll.setWidget(self.senList)
        self.scroll.setWidgetResizable(True)
        self.scroll.setFixedHeight(150)
        self.scroll.setFixedWidth(150)
        layout.addWidget(self.scroll)
        self.scroll.move(10,290)
        self.XIndex = None;

        self.frame8 = QFrame(self)
        self.frame8.setGeometry(QRect(10, 447, 250, 30))
        self.LabelY = QLabel("Y-axis:",self.frame8)
        layout.addWidget(self.LabelY)

        self.frame9 = QFrame(self)
        self.frame9.setGeometry(QRect(10, 465, 300, 200))
        self.YCols = QTableWidget(self.frame9)
        self.YCols.setColumnCount(3)
        self.YCols.setHorizontalHeaderLabels(["L","R",""])
        header = self.YCols.horizontalHeader()
        header.setSectionResizeMode(2, QHeaderView.ResizeMode.Stretch)
        header.setSectionResizeMode(1, QHeaderView.ResizeMode.ResizeToContents)
        header.setSectionResizeMode(0, QHeaderView.ResizeMode.ResizeToContents)
        self.YCols.clicked.connect(self.YSelectionChange)
        self.YCols.setSelectionMode(QAbstractItemView.SelectionMode.NoSelection)
        layout.addWidget(self.YCols)
        self.scroll2 = QScrollArea(self)
        self.scroll2.setWidget(self.YCols)
        self.scroll2.setWidgetResizable(True)
        self.scroll2.setFixedHeight(150)
        self.scroll2.setFixedWidth(270)
        layout.addWidget(self.scroll2)
        self.scroll2.move(10,465)
        self.YIndex = [];
        self.YIndexR = [];

        self.x_pos = 160
        self.dely = 25
        self.w0 = 123
        self.h0 = 25

        y_pos = 110

        self.frame10 = QFrame(self)
        self.frame10.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.linePropBtn = QPushButton("Plot Properties",self.frame10)
        self.linePropBtn.setChecked(True)
        self.linePropBtn.clicked.connect(self.plotProp)
        self.linePropBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.linePropBtn)

        y_pos += self.dely
        self.frame11 = QFrame(self)
        self.frame11.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.axesBtn = QPushButton("Axes",self.frame11)
        self.axesBtn.clicked.connect(self.axesProp)
        self.axesBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.axesBtn)

        y_pos += self.dely
        self.frame12 = QFrame(self)
        self.frame12.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.legendBtn = QPushButton("Legend",self.frame12)
        self.legendBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.legendBtn)
        self.legendBtn.clicked.connect(self.legendProp)

        y_pos += self.dely
        self.frame15 = QFrame(self)
        self.frame15.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.gridBtn = QPushButton("Grid",self.frame15)
        self.gridBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.gridBtn)
        self.gridBtn.clicked.connect(self.gridProp)

        y_pos += self.dely
        self.frame17 = QFrame(self)
        self.frame17.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.titleBtn = QPushButton("Title",self.frame17)
        self.titleBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.titleBtn)
        self.titleBtn.clicked.connect(self.titleProp)

        y_pos += self.dely
        self.frameTicksNt = QFrame(self)
        self.frameTicksNt.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.TicksNtBtn = QPushButton("TickLabelsStyle",self.frameTicksNt)
        self.TicksNtBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.TicksNtBtn)
        self.TicksNtBtn.clicked.connect(self.TicksNtWProp)

        y_pos += self.dely
        self.frameMultiPlot = QFrame(self)
        self.frameMultiPlot.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.multiPlotBtn = QPushButton("MultiPlot",self.frameMultiPlot)
        self.multiPlotBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.multiPlotBtn)
        self.multiPlotBtn.clicked.connect(self.multiPlotProp)

        y_pos += self.dely
        self.frameFourier = QFrame(self)
        self.frameFourier.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.fourierBtn = QPushButton("Fourier",self.frameFourier)
        self.fourierBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.fourierBtn)
        self.fourierBtn.clicked.connect(self.fourierProp)

        y_pos += self.dely
        self.frameAvgRms = QFrame(self)
        self.frameAvgRms.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.avgrmsBtn = QPushButton("Avg/RMS",self.frameAvgRms)
        self.avgrmsBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.avgrmsBtn)
        self.avgrmsBtn.clicked.connect(self.avgrmsProp)

        y_pos += self.dely
        self.framePower = QFrame(self)
        self.framePower.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.powerBtn = QPushButton("Power",self.framePower)
        self.powerBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.powerBtn)
        self.powerBtn.clicked.connect(self.powerProp)

        y_pos += self.dely
        self.frame16 = QFrame(self)
        self.frame16.setGeometry(QRect(self.x_pos, y_pos, self.w0, self.h0))
        self.saveBtn = QPushButton("Save Figure",self.frame16)
        self.saveBtn.setFixedSize(QtCore.QSize(self.w0, self.h0))
        layout.addWidget(self.saveBtn)
        self.saveBtn.clicked.connect(self.openFileSaveDialog)
        self.saveBtn.setEnabled(0)

        self.plotCanvas_1 = PlotCanvas(self, width=5, height=3)
        self.plotCanvas_1.move(285,40)
        self.navi = self.addToolBar(GSEIMPlotNavigationToolbar(self.plotCanvas_1,self))

        self.plotWindow_1 = PlotWindow()
        self.plotWindow_1.move(1,1)
        self.navi2 = self.plotWindow_1.addToolBar(
            GSEIMPlotNavigationToolbar(self.plotWindow_1.canvas, self.plotWindow_1.canvas))

        self.frame13 = QFrame(self)
        self.frame13.setGeometry(QRect(600,520,120,25))
        self.scriptBtn = QPushButton("Generate Script",self.frame13)
        layout.addWidget(self.scriptBtn)
        self.scriptBtn.clicked.connect(self.genScript)
        self.scriptBtn.setEnabled(0)

        x1 = 600
        y1 = 430

        self.frame14 = QFrame(self)
        self.frame14.setGeometry(QRect(400,520,120,25))
        self.pltBtn = QPushButton("Plot Data",self.frame14)
        layout.addWidget(self.pltBtn)
        self.pltBtn.clicked.connect(self.plotWindow)
        self.pltBtn.setEnabled(0)
        self.header = 0
        self.legendEnable = True
        self.linePropPopup_1=LinePropPopup();
        self.linePropPopup_1.setGeometry(QRect(x1, y1, 400, 200))
        self.axesPropObject_1 = AxesPropObject()
        self.axesPopup_1 = AxesPopup()
        self.axesPopup_1.setGeometry(QRect(x1, y1, 600, 180))
        self.axesPropObject_1.setxSNU(3);
        self.axesPropObject_1.setxSNL(-3);
        self.axesPropObject_1.setySNU(3);
        self.axesPropObject_1.setySNL(-3);
        self.axesPropObject_1.setxSNM(True);
        self.axesPropObject_1.setySNM(True);

        self.legendObject_1 = LegendObject();
        self.legendPopup_1 = LegendPopup();
        self.legendPopup_1.setGeometry(QRect(x1, y1, 400, 200))
        self.gridObject_1 = GridObject();
        self.gridPopup_1 = GridPopup();
        self.gridPopup_1.setGeometry(QRect(x1, y1, 400, 200))

        self.multiPlotObject_1 = MultiPlotObject();
        self.multiplotPopup_1 = MultiPlotPopup();
        x_dummy = self.multiplotPopup_1.x_end
        y_dummy = self.multiplotPopup_1.y_end
        self.multiplotPopup_1.setGeometry(QRect(x1, y1, x_dummy, y_dummy))

        self.fourierObject_1 = FourierObject();
        self.fourierPopup_1 = FourierPopup();
        x_dummy = self.fourierPopup_1.x_end
        y_dummy = self.fourierPopup_1.y_end
        self.fourierPopup_1.setGeometry(QRect(x1, y1, x_dummy, y_dummy))

        self.avgrmsObject_1 = AvgRmsObject();
        self.avgrmsPopup_1 = AvgRmsPopup();
        x_dummy = self.avgrmsPopup_1.x_end
        y_dummy = self.avgrmsPopup_1.y_end
        self.avgrmsPopup_1.setGeometry(QRect(x1, y1, x_dummy, y_dummy))

        self.powerObject_1 = PowerObject();
        self.powerPopup_1 = PowerPopup();
        x_dummy = self.powerPopup_1.x_end
        y_dummy = self.powerPopup_1.y_end
        self.powerPopup_1.setGeometry(QRect(x1, y1, x_dummy, y_dummy))

        self.titleObject_1 = TitleObject();
        self.titlePopup_1 = TitlePopup();
        self.titlePopup_1.setGeometry(QRect(x1, y1, 300, 200))

        self.ticksPropObject_x = TicksPropObject();
        self.ticksPropObject_y1 = TicksPropObject();
        self.ticksPropObject_y2 = TicksPropObject();

        self.tickLabelNotation_1 =  TickLabelNotation();
        self.tickLabelNotation_1.setGeometry(QRect(x1, y1, 480, 200))
    def plotWindow(self):
        self.plotWindow_1.setGeometry(QRect(10, 10, 700, 450))
        self.plotWindow_1.show()
    def titleProp(self):
        self.titlePopup_1.TitlePropShow()
        self.titlePopup_1.show();
    def genScript(self):
        self.scriptObject_1 = ScriptObject()
        self.scriptObject_1.setGeometry(QRect(10,10,600,300))
        self.scriptObject_1.show()
    def TicksNtWProp(self):
        self.tickLabelNotation_1.show();
    def legendProp(self):
        self.legendPopup_1.LegendPropShow();
        self.legendPopup_1.show();
    def axesProp(self):
        self.axesPopup_1.AxesPropShow();
        self.axesPopup_1.show();
    def gridProp(self):
        self.gridPopup_1.GridPropShow();
        self.gridPopup_1.show();
    def multiPlotProp(self):
        self.multiplotPopup_1.MultiPlotPropShow();
        self.multiplotPopup_1.show();
    def fourierProp(self):
        self.fourierPopup_1.FourierPropShow();
        self.fourierPopup_1.show();
    def avgrmsProp(self):
        self.avgrmsPopup_1.avgRmsPropShow();
        self.avgrmsPopup_1.show();
    def powerProp(self):
        self.powerPopup_1.powerPropShow();
        self.powerPopup_1.show();
    def plotProp(self):
        self.linePropPopup_1.show()
    def plotDataWithChangedOptions(self):

        if self.multiPlotObject_1.multiPlot:
            self.n_variables = len(self.YIndex) + len(self.YIndexR)
            if self.n_variables == 1:
                self.displayMessage1("Multi-plot error", "Must choose more than one variables.")
                sys.exit()

        self.linePropPopup_1.combo1.clear()

        if self.multiPlotObject_1.multiPlot or self.fourierObject_1.fourier \
           or self.avgrmsObject_1.avg or self.avgrmsObject_1.rms:
            self.n_variables = len(self.YIndex) + len(self.YIndexR)
            if self.n_variables > self.n_variables_max:
                print('ApplicationWindow: plotDataWithChangedOptions:' +
                  '\n   too many variables in multiplot/fourier' +
                  '\n   ignoring multiplot/fourier/avg/rms')
                self.displayMessage1("multiplot error", "Only " + self.n_variables_max + " variables" +
                  " are allowed in multi-plot/fourier/avg/rms")
                return

        if self.multiPlotObject_1.multiPlot:
            if self.avgrmsObject_1.avg or self.avgrmsObject_1.rms:
                for i in range(0,len(self.YIndex)):
                    if (self.plotObject_1[self.YIndex[i]].multiLinLog == 'log'):
                        self.displayMessage1("avg/rms error", "log axis is not allowed in avg/rms")
                        return
                for i in range(0,len(self.YIndexR)):
                    if (self.plotObject_1[self.YIndexR[i]].multiLinLog == 'log'):
                        self.displayMessage1("avg/rms error", "log axis is not allowed in avg/rms")
                        return

        if not (self.fourierObject_1.fourier or self.powerObject_1.compute_power):
            if self.ext == '.csv':
                x = self.reader.iloc[:,self.XIndex]
            if (self.ext == '.dat') | (self.ext == '.txt'):
                x = self.reader[:,self.XIndex]

        self.plotCanvas_1.ax_multi = [None]*self.n_variables_max
        self.plotWindow_1.canvas.ax_multi = [None]*self.n_variables_max

        if self.powerObject_1.compute_power:
            self.displayMessage2("Power computation:" + \
              "\nThe computed data will be shown in graphical format." + \
              "\nIt is also available in this file:\n" + self.filename_power)
            n_plots = 3
            self.plotCanvas_1.ax_power = [None]*n_plots
            self.plotWindow_1.canvas.ax_power = [None]*n_plots

            self.plotCanvas_1.ax2_power = [None]*n_plots
            self.plotWindow_1.canvas.ax2_power = [None]*n_plots

            reader_power = np.loadtxt(self.filename_power)
            t_power = reader_power[:,0]
            v_rms   = reader_power[:,1]
            i_rms   = reader_power[:,2]
            p_avg   = reader_power[:,3]
            p_app   = reader_power[:,4]
            pf      = reader_power[:,5]

            if self.ext == '.csv':
                t = self.reader.iloc[:, 0]
                voltage = self.reader.iloc[:, self.v_index]
                current = self.reader.iloc[:, self.i_index]
            if (self.ext == '.dat') | (self.ext == '.txt'):
                t = self.reader[:, 0]
                voltage = self.reader[:, self.v_index]
                current = self.reader[:, self.i_index]

            self.plotCanvas_1.fig.clf()
            self.plotWindow_1.canvas.fig.clf()

            self.plotCanvas_1.fig.subplots_adjust(hspace=0.0)
            self.plotWindow_1.fig.subplots_adjust(hspace=0.0)

            for i in range(n_plots):
                self.plotCanvas_1.ax_power[i] = self.plotCanvas_1.fig.add_subplot(n_plots, 1, i+1)
                self.plotCanvas_1.ax_power[i].label_outer()
                self.plotCanvas_1.ax_power[i].grid(color='lightgrey')
                self.plotCanvas_1.ax_power[i].legend(loc='best')
                self.plotCanvas_1.ax_power[i].ticklabel_format(axis="x", style="sci",
                    scilimits=(-2, 2),useMathText=True)
                self.plotCanvas_1.ax_power[i].ticklabel_format(axis="y", style="sci",
                    scilimits=(-2, 2),useMathText=True)

                if (self.axesPropObject_1.s_xMin.split()):
                    self.plotCanvas_1.ax_power[i].set_xlim(left = float(self.axesPropObject_1.s_xMin))
                if (self.axesPropObject_1.s_xMax.split()):
                    self.plotCanvas_1.ax_power[i].set_xlim(right = float(self.axesPropObject_1.s_xMax))

                self.plotWindow_1.canvas.ax_power[i] = self.plotWindow_1.fig.add_subplot(n_plots, 1, i+1)
                self.plotWindow_1.canvas.ax_power[i].label_outer()
                self.plotWindow_1.canvas.ax_power[i].grid(color='lightgrey')
                self.plotWindow_1.canvas.ax_power[i].legend(loc='best')
                self.plotWindow_1.canvas.ax_power[i].ticklabel_format(axis="x", style="sci",
                    scilimits=(-2, 2),useMathText=True)
                self.plotWindow_1.canvas.ax_power[i].ticklabel_format(axis="y", style="sci",
                    scilimits=(-2, 2),useMathText=True)

                if (self.axesPropObject_1.s_xMin.split()):
                    self.plotWindow_1.canvas.ax_power[i].set_xlim(left = float(self.axesPropObject_1.s_xMin))
                if (self.axesPropObject_1.s_xMax.split()):
                    self.plotWindow_1.canvas.ax_power[i].set_xlim(right = float(self.axesPropObject_1.s_xMax))

            pl1 = self.plotCanvas_1.ax_power[0].plot(t, voltage,
                 color='dodgerblue',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label=self.v_string)
            self.plotCanvas_1.ax_power[0].plot(t_power, v_rms,
                 color='dodgerblue',
                 linestyle='dashed',
                 linewidth=1.0,
                 drawstyle='default',
                 label='')

            self.plotCanvas_1.ax2_power[0] = self.plotCanvas_1.ax_power[0].twinx()
            pl2 = self.plotCanvas_1.ax2_power[0].plot(t, current,
                 color='orangered',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label=self.i_string)
            self.plotCanvas_1.ax2_power[0].plot(t_power, i_rms,
                 color='orangered',
                 linestyle='dashed',
                 linewidth=1.0,
                 drawstyle='default',
                 label='')
            pls = pl1 + pl2
            labs = [l.get_label() for l in pls]
            self.plotCanvas_1.ax_power[0].legend(pls, labs, loc='best')

            self.plotCanvas_1.ax_power[1].plot(t_power, p_avg,
                 color='teal',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='p_avg')
            self.plotCanvas_1.ax_power[1].plot(t_power, p_app,
                 color='deeppink',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='p_app')
            self.plotCanvas_1.ax_power[1].legend()

            self.plotCanvas_1.ax_power[2].plot(t_power, pf,
                 color='darkgoldenrod',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='pf')
            self.plotCanvas_1.ax_power[2].legend()

            pl1w = self.plotWindow_1.canvas.ax_power[0].plot(t, voltage,
                 color='dodgerblue',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label="'" + self.v_string + "'")
            self.plotWindow_1.canvas.ax_power[0].plot(t_power, v_rms,
                 color='dodgerblue',
                 linestyle='dashed',
                 linewidth=1.0,
                 drawstyle='default',
                 label='')
            self.plotWindow_1.canvas.ax_power[0].legend()

            self.plotWindow_1.canvas.ax2_power[0] = self.plotWindow_1.canvas.ax_power[0].twinx()
            pl2w = self.plotWindow_1.canvas.ax2_power[0].plot(t, current,
                 color='orangered',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label="'" + self.i_string + "'")
            self.plotWindow_1.canvas.ax2_power[0].plot(t_power, i_rms,
                 color='orangered',
                 linestyle='dashed',
                 linewidth=1.0,
                 drawstyle='default',
                 label='')
            plsw = pl1w + pl2w
            self.plotWindow_1.canvas.ax_power[0].legend(plsw, labs, loc='best')

            self.plotWindow_1.canvas.ax_power[1].plot(t_power, p_avg,
                 color='teal',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='p_avg')
            self.plotWindow_1.canvas.ax_power[1].plot(t_power, p_app,
                 color='deeppink',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='p_app')
            self.plotWindow_1.canvas.ax_power[1].legend()

            self.plotWindow_1.canvas.ax_power[2].plot(t_power, pf,
                 color='darkgoldenrod',
                 linestyle='solid',
                 linewidth=1.0,
                 drawstyle='default',
                 label='pf')
            self.plotWindow_1.canvas.ax_power[2].legend()

            self.plotCanvas_1.draw()
            self.plotWindow_1.draw()
        elif self.fourierObject_1.fourier:
#           It would not make sense to plot different variables in a single plot;
#           assume multi-plot even if it is not specified.

            self.displayMessage2("Fourier coefficients:" + \
              "\nThe computed data will be shown in graphical format." + \
              "\nIt is also available in these files:" + \
              "\n" + self.filename_fourier + \
              "\n" + self.filename_thd + "\n")

            reader_fourier = np.loadtxt(self.filename_fourier)
            x = reader_fourier[:,0]

            thd = np.loadtxt(self.filename_thd)
            print("ApplicationWindow: plotDataWithChangedOptions: thd:", thd)

            self.plotCanvas_1.fig.clf()
            self.plotWindow_1.fig.clf()

            self.plotCanvas_1.fig.subplots_adjust(hspace=0.0)
            self.plotWindow_1.fig.subplots_adjust(hspace=0.0)

            for i in range(self.n_variables):
                self.plotCanvas_1.ax_multi[i] = self.plotCanvas_1.fig.add_subplot(self.n_variables, 1, i+1)
                self.plotCanvas_1.ax_multi[i].label_outer()
                self.plotCanvas_1.ax_multi[i].grid(color='lightgrey')
                self.plotCanvas_1.ax_multi[i].legend(loc='best')
                self.plotCanvas_1.ax_multi[i].ticklabel_format(axis="y", style="sci",
                    scilimits=(-2, 2),useMathText=True)
                self.plotCanvas_1.ax_multi[i].set_axisbelow(True)
                self.plotCanvas_1.ax_multi[i].xaxis.set_major_locator(MaxNLocator(integer=True))

                self.plotWindow_1.ax_multi[i] = self.plotWindow_1.fig.add_subplot(self.n_variables, 1, i+1)
                self.plotWindow_1.ax_multi[i].label_outer()
                self.plotWindow_1.ax_multi[i].grid(color='lightgrey')
                self.plotWindow_1.ax_multi[i].legend(loc='best')
                self.plotWindow_1.ax_multi[i].ticklabel_format(axis="y", style="sci",
                    scilimits=(-2, 2),useMathText=True)
                self.plotWindow_1.ax_multi[i].set_axisbelow(True)
                self.plotWindow_1.ax_multi[i].xaxis.set_major_locator(MaxNLocator(integer=True))

            if self.n_variables == 1:
                y_thd = 0.88
            elif self.n_variables == 2:
                y_thd = 0.78
            elif self.n_variables == 3:
                y_thd = 0.68

            i_ax = 0
            bar_width = 0.5
            for i in range(0,len(self.YIndex)):
                y1 = reader_fourier[:,(i_ax+1)]

                self.plotCanvas_1.ax_multi[i_ax].bar(x, y1, width=bar_width, color='darkorange',align='center',
                   label=self.plotObject_1[self.YIndex[i]].label)
                self.plotCanvas_1.ax_multi[i_ax].legend()

                if thd.ndim > 0:
                    thd1 = thd[i_ax]
                else:
                    thd1 = thd.tolist()
                s = "THD: "
                s += "%6.2f %%"%(100.0*thd1)
                self.plotCanvas_1.ax_multi[i_ax].text(0.9, y_thd, s,
                   horizontalalignment='center',
                   verticalalignment='top',
                   transform=self.plotCanvas_1.ax_multi[i_ax].transAxes)

                self.plotWindow_1.ax_multi[i_ax].bar(x, y1, width=bar_width, color='darkorange',align='center',
                   label=self.plotObject_1[self.YIndex[i]].label)
                self.plotWindow_1.ax_multi[i_ax].legend()
                self.plotWindow_1.ax_multi[i_ax].text(0.9, y_thd, s,
                   horizontalalignment='center',
                   verticalalignment='top',
                   transform=self.plotCanvas_1.ax_multi[i_ax].transAxes)

                self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                i_ax += 1
            for i in range(0,len(self.YIndexR)):
                y1 = reader_fourier[:,(i_ax+1)]

                self.plotCanvas_1.ax_multi[i_ax].bar(x, y1, width=bar_width, color='darkorange',align='center',
                   label=self.plotObject_1[self.YIndexR[i]].label)
                self.plotCanvas_1.ax_multi[i_ax].legend()
                if thd.ndim > 0:
                    thd1 = thd[i_ax]
                else:
                    thd1 = thd.tolist()
                s = "THD: "
                s += "%6.2f %%"%(100.0*thd1)
                self.plotCanvas_1.ax_multi[i_ax].text(0.9, y_thd, s,
                   horizontalalignment='center',
                   verticalalignment='top',
                   transform=self.plotCanvas_1.ax_multi[i_ax].transAxes)

                self.plotWindow_1.ax_multi[i_ax].bar(x, y1, width=bar_width, color='darkorange',align='center',
                   label=self.plotObject_1[self.YIndexR[i]].label)
                self.plotWindow_1.ax_multi[i_ax].legend()
                self.plotWindow_1.ax_multi[i_ax].text(0.9, y_thd, s,
                   horizontalalignment='center',
                   verticalalignment='top',
                   transform=self.plotCanvas_1.ax_multi[i_ax].transAxes)

                self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndexR[i],2).text())
                i_ax += 1

            self.plotCanvas_1.draw()
            self.plotWindow_1.draw()

        elif self.multiPlotObject_1.multiPlot:
            self.plotCanvas_1.fig.clf()
            self.plotWindow_1.fig.clf()

            if self.avgrmsObject_1.avg and self.avgrmsObject_1.rms:
                self.displayMessage2("avg/rms:" + \
                  "\nThe computed data will be shown in graphical format." + \
                  "\nIt is also available in these files:" + \
                  "\n" + self.filename_avg + \
                  "\n" + self.filename_rms + "\n")
            elif self.avgrmsObject_1.avg:
                self.displayMessage2("avg:" + \
                  "\nThe computed data will be shown in graphical format." + \
                  "\nIt is also available in this file:" + \
                  "\n" + self.filename_avg + "\n")
            elif self.avgrmsObject_1.rms:
                self.displayMessage2("rms:" + \
                  "\nThe computed data will be shown in graphical format." + \
                  "\nIt is also available in this file:" + \
                  "\n" + self.filename_rms + "\n")

            self.plotCanvas_1.fig.subplots_adjust(hspace=0.0)
            self.plotWindow_1.fig.subplots_adjust(hspace=0.0)

            for i in range(self.n_variables):
                if i == 0:
                    self.plotCanvas_1.ax_multi[i] = self.plotCanvas_1.fig.add_subplot(self.n_variables, 1, i+1)
                else:
                    self.plotCanvas_1.ax_multi[i] = self.plotCanvas_1.fig.add_subplot(self.n_variables, 1, i+1,
                      sharex=self.plotCanvas_1.ax_multi[0])
                self.plotCanvas_1.ax_multi[i].label_outer()
                self.plotCanvas_1.ax_multi[i].grid(color='lightgrey')
                self.plotCanvas_1.ax_multi[i].legend(loc='best')
                self.plotCanvas_1.ax_multi[i].ticklabel_format(axis="x", style="sci",
                    scilimits=(-2, 2),useMathText=True)

                if (self.axesPropObject_1.s_xMin.split()):
                    self.plotCanvas_1.ax_multi[i].set_xlim(left = float(self.axesPropObject_1.s_xMin))
                if (self.axesPropObject_1.s_xMax.split()):
                    self.plotCanvas_1.ax_multi[i].set_xlim(right = float(self.axesPropObject_1.s_xMax))

                if i == 0:
                    self.plotWindow_1.ax_multi[i] = self.plotWindow_1.fig.add_subplot(self.n_variables, 1, i+1)
                else:
                    self.plotWindow_1.ax_multi[i] = self.plotWindow_1.fig.add_subplot(self.n_variables, 1, i+1,
                      sharex=self.plotWindow_1.ax_multi[0])
                self.plotWindow_1.ax_multi[i].label_outer()
                self.plotWindow_1.ax_multi[i].grid(color='lightgrey')
                self.plotWindow_1.ax_multi[i].legend(loc='best')
                self.plotWindow_1.ax_multi[i].ticklabel_format(axis="x", style="sci",
                    scilimits=(-2, 2),useMathText=True)

                if (self.axesPropObject_1.s_xMin.split()):
                    self.plotWindow_1.ax_multi[i].set_xlim(left = float(self.axesPropObject_1.s_xMin))
                if (self.axesPropObject_1.s_xMax.split()):
                    self.plotWindow_1.ax_multi[i].set_xlim(right = float(self.axesPropObject_1.s_xMax))

            if not (self.avgrmsObject_1.avg or self.avgrmsObject_1.rms):
                i_ax = 0
                for i in range(0,len(self.YIndex)):
                    pltColor = self.colorSet[(self.YIndex[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndex[i]].lineStyle == None:
                        self.plotObject_1[self.YIndex[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndex[i]].lineColor == None:
                        self.plotObject_1[self.YIndex[i]].setLineColor(pltColor)
                    if self.ext == '.csv':
                        y1 = self.reader.iloc[:,self.YIndex[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y1 = self.reader[:,self.YIndex[i]]
                    if (self.plotObject_1[self.YIndex[i]].multiLinLog == 'log'):
                        y = np.log10(y1)
                    else:
                        y = y1
                    self.plotCanvas_1.ax_multi[i_ax].plot(x,y,
                         color=self.plotObject_1[self.YIndex[i]].lineColor,
                         linestyle=self.plotObject_1[self.YIndex[i]].lineStyle,
                         linewidth=self.plotObject_1[self.YIndex[i]].width,
                         drawstyle=self.plotObject_1[self.YIndex[i]].drawStyle,
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    self.plotCanvas_1.ax_multi[i_ax].legend()
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                    if self.plotObject_1[self.YIndex[i]].multiLinLog == 'linear':
                        self.plotCanvas_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                            scilimits=(-2, 2),useMathText=True)
                    self.plotWindow_1.ax_multi[i_ax].plot(x,y,
                         color=self.plotObject_1[self.YIndex[i]].lineColor,
                         linestyle=self.plotObject_1[self.YIndex[i]].lineStyle,
                         linewidth=self.plotObject_1[self.YIndex[i]].width,
                         drawstyle=self.plotObject_1[self.YIndex[i]].drawStyle,
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    self.plotWindow_1.ax_multi[i_ax].legend()
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                    if self.plotObject_1[self.YIndex[i]].multiLinLog == 'linear':
                        self.plotWindow_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                            scilimits=(-2, 2),useMathText=True)
                    i_ax += 1
                for i in range(0,len(self.YIndexR)):
                    pltColor = self.colorSet[(self.YIndexR[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndexR[i]].lineStyle == None:
                        self.plotObject_1[self.YIndexR[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndexR[i]].lineColor == None:
                        self.plotObject_1[self.YIndexR[i]].setLineColor(pltColor)
                    if self.ext == '.csv':
                        y1 = self.reader.iloc[:,self.YIndexR[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y1 = self.reader[:,self.YIndexR[i]]
                    if (self.plotObject_1[self.YIndexR[i]].multiLinLog == 'log'):
                        y = np.log10(y1)
                    else:
                        y = y1
                    self.plotCanvas_1.ax_multi[i_ax].plot(x,y,
                         color=self.plotObject_1[self.YIndexR[i]].lineColor,
                         linestyle=self.plotObject_1[self.YIndexR[i]].lineStyle,
                         linewidth=self.plotObject_1[self.YIndexR[i]].width,
                         drawstyle=self.plotObject_1[self.YIndexR[i]].drawStyle,
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    self.plotCanvas_1.ax_multi[i_ax].legend()
                    if self.plotObject_1[self.YIndexR[i]].multiLinLog == 'linear':
                        self.plotCanvas_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                            scilimits=(-2, 2),useMathText=True)
                    self.plotWindow_1.ax_multi[i_ax].plot(x,y,
                         color=self.plotObject_1[self.YIndexR[i]].lineColor,
                         linestyle=self.plotObject_1[self.YIndexR[i]].lineStyle,
                         linewidth=self.plotObject_1[self.YIndexR[i]].width,
                         drawstyle=self.plotObject_1[self.YIndexR[i]].drawStyle,
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    self.plotWindow_1.ax_multi[i_ax].legend()
                    if self.plotObject_1[self.YIndexR[i]].multiLinLog == 'linear':
                        self.plotWindow_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                            scilimits=(-2, 2),useMathText=True)
                    i_ax += 1
            else:

                if self.avgrmsObject_1.avg:
                    reader_avg = np.loadtxt(self.filename_avg)
                    t_avg = reader_avg[:,0]
                if self.avgrmsObject_1.rms:
                    reader_rms = np.loadtxt(self.filename_rms)
                    t_rms = reader_rms[:,0]

                i_ax = 0
                for i in range(0,len(self.YIndex)):
                    if self.avgrmsObject_1.avg:
                        y_avg = reader_avg[:,(i_ax+1)]
                    if self.avgrmsObject_1.rms:
                        y_rms = reader_rms[:,(i_ax+1)]
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndex[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndex[i]]

                    pltColor = self.colorSet[(self.YIndex[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndex[i]].lineStyle == None:
                        self.plotObject_1[self.YIndex[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndex[i]].lineColor == None:
                        self.plotObject_1[self.YIndex[i]].setLineColor(pltColor)

                    self.plotCanvas_1.ax_multi[i_ax].plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotCanvas_1.ax_multi[i_ax].plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotCanvas_1.ax_multi[i_ax].plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotCanvas_1.ax_multi[i_ax].legend(loc='best')
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                    self.plotCanvas_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                        scilimits=(-2, 2),useMathText=True)

                    self.plotWindow_1.ax_multi[i_ax].plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotWindow_1.ax_multi[i_ax].plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotWindow_1.ax_multi[i_ax].plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotWindow_1.ax_multi[i_ax].legend(loc='best')
                    self.plotWindow_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                        scilimits=(-2, 2),useMathText=True)

                    i_ax += 1
                for i in range(0,len(self.YIndexR)):
                    if self.avgrmsObject_1.avg:
                        y_avg = reader_avg[:,(i_ax+1)]
                    if self.avgrmsObject_1.rms:
                        y_rms = reader_rms[:,(i_ax+1)]
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndexR[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndexR[i]]

                    pltColor = self.colorSet[(self.YIndexR[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndexR[i]].lineStyle == None:
                        self.plotObject_1[self.YIndexR[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndexR[i]].lineColor == None:
                        self.plotObject_1[self.YIndexR[i]].setLineColor(pltColor)

                    self.plotCanvas_1.ax_multi[i_ax].plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotCanvas_1.ax_multi[i_ax].plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotCanvas_1.ax_multi[i_ax].plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotCanvas_1.ax_multi[i_ax].legend(loc='best')
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndexR[i],2).text())
                    self.plotCanvas_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                        scilimits=(-2, 2),useMathText=True)

                    self.plotWindow_1.ax_multi[i_ax].plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotWindow_1.ax_multi[i_ax].plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotWindow_1.ax_multi[i_ax].plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotWindow_1.ax_multi[i_ax].legend(loc='best')
                    self.plotWindow_1.ax_multi[i_ax].ticklabel_format(axis="y", style="sci",
                        scilimits=(-2, 2),useMathText=True)

                    i_ax += 1

            self.plotCanvas_1.draw()
            self.plotWindow_1.draw()
        else:
            self.plotCanvas_1.fig.clf()
            self.plotWindow_1.canvas.fig.clf()

            for i in range(self.n_variables_max):
                if self.plotCanvas_1.ax_multi[i]:
                    self.plotCanvas_1.ax_multi[i].remove()
            for i in range(self.n_variables_max):
                if self.plotWindow_1.canvas.ax_multi[i]:
                    self.plotWindow_1.canvas.ax_multi[i].remove()

            self.plotCanvas_1.ax = self.plotCanvas_1.fig.add_subplot(1, 1, 1)
            self.plotCanvas_1.ax.grid()
            self.plotCanvas_1.ax2 = self.plotCanvas_1.ax.twinx()

            self.plotWindow_1.canvas.ax = self.plotWindow_1.canvas.fig.add_subplot(1, 1, 1)
            self.plotWindow_1.canvas.ax.grid()
            self.plotWindow_1.canvas.ax2 = self.plotWindow_1.canvas.ax.twinx()

            if not (self.avgrmsObject_1.avg or self.avgrmsObject_1.rms):
                self.pls  = [];self.plbs = [];
                for i in range(0,len(self.YIndex)):
                    pltColor = self.colorSet[(self.YIndex[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndex[i]].lineStyle == None:
                        self.plotObject_1[self.YIndex[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndex[i]].lineColor == None:
                        self.plotObject_1[self.YIndex[i]].setLineColor(pltColor)
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndex[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndex[i]]
                    pl = self.plotCanvas_1.plotData(x,y,
                         self.plotObject_1[self.YIndex[i]].lineColor,
                         self.plotObject_1[self.YIndex[i]].lineStyle,
                         self.plotObject_1[self.YIndex[i]].width,
                         self.plotObject_1[self.YIndex[i]].drawStyle,
                         self.plotObject_1[self.YIndex[i]].label,
                         self.plotObject_1[self.YIndex[i]].marker,
                         self.plotObject_1[self.YIndex[i]].size,
                         self.plotObject_1[self.YIndex[i]].edgeColor,
                         self.plotObject_1[self.YIndex[i]].faceColor);
                    self.pls.append(pl);self.plbs.append(self.plotObject_1[self.YIndex[i]].label);
                    self.plotWindow_1.canvas.plotData(x,y,
                         self.plotObject_1[self.YIndex[i]].lineColor,
                         self.plotObject_1[self.YIndex[i]].lineStyle,
                         self.plotObject_1[self.YIndex[i]].width,
                         self.plotObject_1[self.YIndex[i]].drawStyle,
                         self.plotObject_1[self.YIndex[i]].label,
                         self.plotObject_1[self.YIndex[i]].marker,
                         self.plotObject_1[self.YIndex[i]].size,
                         self.plotObject_1[self.YIndex[i]].edgeColor,
                         self.plotObject_1[self.YIndex[i]].faceColor);
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                    pixmap = QPixmap(10,10);
                    pixmap.fill(QColor(self.plotObject_1[self.YIndex[i]].lineColor));
                    self.redIcon= QIcon(pixmap);
                    self.YCols.item(self.YIndex[i],0).setIcon(QIcon(self.redIcon))

                for i in range(0,len(self.YIndexR)):
                    pltColor = self.colorSet[(self.YIndexR[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndexR[i]].lineStyle == None:
                        self.plotObject_1[self.YIndexR[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndexR[i]].lineColor == None:
                        self.plotObject_1[self.YIndexR[i]].setLineColor(pltColor)
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndexR[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndexR[i]]
                    pl = self.plotCanvas_1.plotData(x,y,
                        self.plotObject_1[self.YIndexR[i]].lineColor,
                        self.plotObject_1[self.YIndexR[i]].lineStyle,
                        self.plotObject_1[self.YIndexR[i]].width,
                        self.plotObject_1[self.YIndexR[i]].drawStyle,
                        self.plotObject_1[self.YIndexR[i]].label,
                        self.plotObject_1[self.YIndexR[i]].marker,
                        self.plotObject_1[self.YIndexR[i]].size,
                        self.plotObject_1[self.YIndexR[i]].edgeColor,
                        self.plotObject_1[self.YIndexR[i]].faceColor,yax='right');
                    self.pls.append(pl);self.plbs.append(self.plotObject_1[self.YIndexR[i]].label);
                    self.plotWindow_1.canvas.plotData(x,y,
                        self.plotObject_1[self.YIndexR[i]].lineColor,
                        self.plotObject_1[self.YIndexR[i]].lineStyle,
                        self.plotObject_1[self.YIndexR[i]].width,
                        self.plotObject_1[self.YIndexR[i]].drawStyle,
                        self.plotObject_1[self.YIndexR[i]].label,
                        self.plotObject_1[self.YIndexR[i]].marker,
                        self.plotObject_1[self.YIndexR[i]].size,
                        self.plotObject_1[self.YIndexR[i]].edgeColor,
                        self.plotObject_1[self.YIndexR[i]].faceColor,yax='right');
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndexR[i],2).text())
                    pixmap = QPixmap(10,10);
                    pixmap.fill(QColor(self.plotObject_1[self.YIndexR[i]].lineColor));
                    self.redIcon= QIcon(pixmap);
                    self.YCols.item(self.YIndexR[i],1).setIcon(QIcon(self.redIcon))
            else:
                if self.avgrmsObject_1.avg:
                    reader_avg = np.loadtxt(self.filename_avg)
                    t_avg = reader_avg[:,0]
                if self.avgrmsObject_1.rms:
                    reader_rms = np.loadtxt(self.filename_rms)
                    t_rms = reader_rms[:,0]
                i_ax = 0
                for i in range(0,len(self.YIndex)):
                    pltColor = self.colorSet[(self.YIndex[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndex[i]].lineStyle == None:
                        self.plotObject_1[self.YIndex[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndex[i]].lineColor == None:
                        self.plotObject_1[self.YIndex[i]].setLineColor(pltColor)

                    if self.avgrmsObject_1.avg:
                        y_avg = reader_avg[:,(i_ax+1)]
                    if self.avgrmsObject_1.rms:
                        y_rms = reader_rms[:,(i_ax+1)]
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndex[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndex[i]]

                    self.plotCanvas_1.ax.plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotCanvas_1.ax.plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotCanvas_1.ax.plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')

                    self.plotCanvas_1.ax.legend()
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndex[i],2).text())
                    pixmap = QPixmap(10,10);
                    pixmap.fill(QColor(self.plotObject_1[self.YIndex[i]].lineColor));
                    self.redIcon= QIcon(pixmap);
                    self.YCols.item(self.YIndex[i],0).setIcon(QIcon(self.redIcon))

                    self.plotWindow_1.canvas.ax.plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndex[i]].label,
                         marker=self.plotObject_1[self.YIndex[i]].marker,
                         markersize=self.plotObject_1[self.YIndex[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndex[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndex[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotWindow_1.canvas.ax.plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotWindow_1.canvas.ax.plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotWindow_1.canvas.ax.legend()
                    i_ax += 1
                for i in range(0,len(self.YIndexR)):
                    pltColor = self.colorSet[(self.YIndexR[i])%self.nColorSet]
                    if self.plotObject_1[self.YIndexR[i]].lineStyle == None:
                        self.plotObject_1[self.YIndexR[i]].setLineStyle('solid')
                    if self.plotObject_1[self.YIndexR[i]].lineColor == None:
                        self.plotObject_1[self.YIndexR[i]].setLineColor(pltColor)

                    if self.avgrmsObject_1.avg:
                        y_avg = reader_avg[:,(i_ax+1)]
                    if self.avgrmsObject_1.rms:
                        y_rms = reader_rms[:,(i_ax+1)]
                    if self.ext == '.csv':
                        y = self.reader.iloc[:,self.YIndexR[i]]
                    if (self.ext == '.dat') | (self.ext == '.txt'):
                        y = self.reader[:,self.YIndexR[i]]

                    self.plotCanvas_1.ax2.plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotCanvas_1.ax2.plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotCanvas_1.ax2.plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')

                    self.plotCanvas_1.ax2.legend()
                    self.linePropPopup_1.combo1.addItem(self.YCols.item(self.YIndexR[i],2).text())
                    pixmap = QPixmap(10,10);
                    pixmap.fill(QColor(self.plotObject_1[self.YIndexR[i]].lineColor));
                    self.redIcon= QIcon(pixmap);
                    self.YCols.item(self.YIndexR[i],0).setIcon(QIcon(self.redIcon))

                    self.plotWindow_1.canvas.ax2.plot(x,y,
                         color=pltColor,
                         linestyle='solid',
                         linewidth=1.0,
                         drawstyle='default',
                         label=self.plotObject_1[self.YIndexR[i]].label,
                         marker=self.plotObject_1[self.YIndexR[i]].marker,
                         markersize=self.plotObject_1[self.YIndexR[i]].size,
                         markeredgecolor=self.plotObject_1[self.YIndexR[i]].edgeColor,
                         markerfacecolor=self.plotObject_1[self.YIndexR[i]].faceColor)
                    if self.avgrmsObject_1.avg:
                        self.plotWindow_1.canvas.ax2.plot(t_avg,y_avg,
                             color=pltColor,
                             linestyle='dashed',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    if self.avgrmsObject_1.rms:
                        self.plotWindow_1.canvas.ax2.plot(t_rms,y_rms,
                             color=pltColor,
                             linestyle='dashdot',
                             linewidth=1.0,
                             drawstyle='default',
                             label='')
                    self.plotWindow_1.canvas.ax2.legend()
                    i_ax += 1

            if self.legendEnable:
                self.plotCanvas_1.showLegend();
                self.plotWindow_1.canvas.showLegend();
            self.axesPropObject_1.setxLabel(self.senList.item(self.XIndex).text())

            self.plotCanvas_1.changeAxesProps();
            self.plotWindow_1.canvas.changeAxesProps();

            self.linePropBtn.setEnabled(1)
            self.axesBtn.setEnabled(1)
            self.legendBtn.setEnabled(1)
            self.gridBtn.setEnabled(1)
            self.saveBtn.setEnabled(1)

            self.plotCanvas_1.changeGridProps();
            self.plotWindow_1.canvas.changeGridProps();

            self.titleBtn.setEnabled(1)
            self.plotCanvas_1.changeTitle()
            self.plotWindow_1.canvas.changeTitle();
            self.plotCanvas_1.getxTicks();

    def FileSelectionChange(self):
        NoOfItems = self.OPFiles.count()
        prev = self.FileIndex;
        listOfFileIndex = []
        for i in range(0,NoOfItems):
            item=self.OPFiles.item(i)
            if item.checkState():
                listOfFileIndex.append(i)
        if len(listOfFileIndex)>1:
            for i in range(0,len(listOfFileIndex)):
                if self.FileIndex == listOfFileIndex[i]:
                    self.OPFiles.item(listOfFileIndex[i]).setCheckState(QtCore.Qt.CheckState.Unchecked)
                else:
                    prev = listOfFileIndex[i]
        else:
            if len(listOfFileIndex) ==0:
                prev = None
            else:
                prev = listOfFileIndex[0]
        if self.FileIndex != prev:
            self.FileIndex = prev
            if self.FileIndex == None:
                NoOfItem =self.senList.count()
                self.label.setEnabled(0)
                self.labelR.setEnabled(0)
                self.scriptBtn.setEnabled(0)
                for ic in range(0,NoOfItem):
                    self.senList.takeItem(0)
                    self.YCols.takeItem(ic,2);
                    self.YCols.takeItem(ic,1);
                    self.YCols.takeItem(ic,0);
                self.YCols.setRowCount(0)
                self.fileName = None
            else:
                self.fileName = self.dir_output_files + self.DataFiles[prev]
                if path.exists(self.fileName):
                    self.readFile();
                else:
                    self.labelR.setEnabled(0)
                    self.label.setEnabled(0)

                    self.displayMessage2("File Error:" + \
                        "\nFile " + self.fileName + \
                        "\ndoes not exist.")

    def XSelectionChange(self):
        NoOfItems = self.senList.count()
        prev = self.XIndex;
        listOfXIndex = []
        for i in range(0,NoOfItems):
            item=self.senList.item(i)
            if item.checkState():
                listOfXIndex.append(i)
        if len(listOfXIndex)>1:
            for i in range(0,len(listOfXIndex)):
                if self.XIndex == listOfXIndex[i]:
                    self.senList.item(listOfXIndex[i]).setCheckState(QtCore.Qt.CheckState.Unchecked)
                else:
                    prev = listOfXIndex[i]

        else:
            if len(listOfXIndex) ==0:
                prev = None
            else:
                prev = listOfXIndex[0]

        if self.XIndex != prev:
            self.XIndex = prev
            self.plotDataWithChangedOptions();
    def YSelectionChange(self):
        NoOfItems = self.n_col#YCols.count()
        prev = self.YIndex;
        listOfYIndex = []
        listOfYIndexR = []
        for i in range(0,NoOfItems):
            item=self.YCols.item(i,0)
            item2=self.YCols.item(i,1)
            pixmap = QPixmap(10,10);
            pixmap.fill(QColor("white"));
            self.redIcon= QIcon(pixmap);

            checked = QtCore.Qt.CheckState.Checked
            if (item.checkState()==checked and item2.checkState()!=checked) or (item.checkState()==checked and \
                (self.FlagLR[i] == 'N' or self.FlagLR[i] == 'R')):
                listOfYIndex.append(i)
                self.FlagLR[i]='L'
                self.YCols.item(i,1).setCheckState(QtCore.Qt.CheckState.Unchecked)
                self.YCols.item(i,1).setIcon(QIcon(self.redIcon))

            else:
                self.YCols.item(i,0).setIcon(QIcon(self.redIcon))

            if (item2.checkState()==checked and (item.checkState()!=checked)) or \
               (item2.checkState()==checked and (self.FlagLR[i] == 'N' or self.FlagLR[i] == 'L')):
                listOfYIndexR.append(i)
                self.FlagLR[i]='R'
                self.YCols.item(i,0).setCheckState(QtCore.Qt.CheckState.Unchecked)
                self.YCols.item(i,0).setIcon(QIcon(self.redIcon))
            else:
                self.YCols.item(i,1).setIcon(QIcon(self.redIcon))
                self.YCols.item(i,1).setCheckState(QtCore.Qt.CheckState.Unchecked)

        if self.YIndex != listOfYIndex or self.YIndexR != listOfYIndexR:
            self.YIndex = listOfYIndex
            self.YIndexR = listOfYIndexR
            if self.XIndex == None:
                self.linePropPopup_1.combo1.clear()
            else:
                self.plotDataWithChangedOptions();

    def openFileNameDialog(self):
        options = QFileDialog.Option.DontUseNativeDialog#"All Files (*);;Python Files (*.py)"*.xlsx *.csv

        if path.exists(self.file_last_opened):
            with open(os.path.expanduser(self.file_last_opened),'r') as f:
                    netlistlines=f.readlines()
            self.DefaultPath=netlistlines[0]
        else:
            self.DefaultPath=""

        self.ProjectfileName, _ = QFileDialog.getOpenFileName(self,"QFileDialog.getOpenFileName()", \
            self.DefaultPath," *.in", options=options)
        print(self.ProjectfileName)
        if path.exists(self.ProjectfileName):
            self.labelFile.setText(self.ProjectfileName)
            self.labelFile.setEnabled(1)
            self.labelFilen.setText(self.ProjectfileName[self.ProjectfileName.rindex('/')+1:])
            self.labelFilen.setEnabled(1)
            self.labelR.setEnabled(0)
            self.label.setEnabled(0)
            self.SolveBlkLine = []
            self.DefaultPath = self.ProjectfileName[:self.ProjectfileName.rindex('/')]
            with open(os.path.expanduser(self.file_last_opened), 'w+') as filetowrite:
                filetowrite.write(self.DefaultPath)
            self.netlist = self.dir_output_files + \
                self.ProjectfileName[self.ProjectfileName.rindex('/'):self.ProjectfileName.rindex('.')]+'.in'
            self.netlist = self.netlist.replace('//', '/')

            self.avgrmsObject_1.setAvg(False)
            self.avgrmsObject_1.setRms(False)
            self.powerObject_1.setComputePower(False)
            self.fourierObject_1.setFourier(False)
            self.multiPlotObject_1.setMultiPlot(False)

            if path.exists(self.netlist):
                NoOfOPFiles =self.OPFiles.count()
                self.FileIndex = None
                for ic in range(0,NoOfOPFiles):
                    self.OPFiles.takeItem(0)

                with open(os.path.expanduser(self.netlist),'r') as f:
                    netlistlines=f.readlines()
                    self.SolveBlkLine = []
                    self.SolveBlkEndLine = []
                    self.OPBlkLine = []
                    self.OPBlkEndLine = []
                    self.DataFiles =[]

                    LineNumber = 0
                    for line in netlistlines:
                        if 'begin_solve' in line:
                            self.SolveBlkLine.append(LineNumber)
                        if 'end_solve' in line:
                            self.SolveBlkEndLine.append(LineNumber)
                        if 'begin_output' in line:
                            flag_sol_file = False
                            if '.dat' in netlistlines[LineNumber+1]:
                                self.OPBlkLine.append(LineNumber)
                                opFileLine = netlistlines[LineNumber+1]
                                dfile = opFileLine[opFileLine.index('=')+1:opFileLine.index('.dat')+4]
                                self.DataFiles.append(dfile)

                                itm = QListWidgetItem()
                                itm.setText(dfile)
                                itm.setFlags(itm.flags() | QtCore.Qt.ItemFlag.ItemIsUserCheckable)
                                itm.setCheckState(QtCore.Qt.CheckState.Unchecked)

                                self.OPFiles.addItem(itm)
                            else:
                                flag_sol_file = True
                        if 'end_output' in line:
                            if not flag_sol_file:
                                self.OPBlkEndLine.append(LineNumber)
                        LineNumber+=1

                    iblk =0;
                    self.variables=[]
                    self.varCnt = []
                    if len(self.SolveBlkLine)==0:
                        self.msg.setText("No Solve blocks for selected project!")
                        self.showWarningDialog();
                    else:
                        for blk in self.OPBlkLine:
                            for ln in range(blk, self.OPBlkEndLine[iblk]):
                                lin = netlistlines[ln]
                                if 'variables:' in lin:
                                    var = lin[lin.index(':')+2:-1].split(' ')
                                    self.variables.append(var)
                                    self.varCnt.append(len(var))
                            iblk+=1

                NoOfItem =self.senList.count()
                for ic in range(0,NoOfItem):
                    self.senList.takeItem(0)
                    self.YCols.takeItem(ic,2);
                    self.YCols.takeItem(ic,1);
                    self.YCols.takeItem(ic,0);
                self.YCols.setRowCount(0)
            else:
                self.displayMessage2("File Error:" + \
                    "\nFile " + self.fileName + \
                    "\ndoes not exist.")

    def openFileSaveDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self.fileName, _ = QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()",
            "","*.eps *.dat *.pgf *.pdf *.ps *.raw *.svg *.tiff", options=options)
        ax = self.plotCanvas_1.fig.axes;
        self.plotCanvas_1.fig.savefig(self.fileName)
    def showWarningDialog(self):
        self.msg.setIcon(QMessageBox.Icon.Information)
        self.msg.setWindowTitle("Warning!")
        self.msg.setStandardButtons(QMessageBox.StandardButton.Ok | QMessageBox.StandardButton.Cancel)
        returnValue = self.msg.exec()
    def displayMessage1(self, message_1, message_2):
        self.msg.setIcon(QMessageBox.Icon.Information)
        self.msg.setText(message_1)
        self.msg.setWindowTitle("Message")
        self.msg.setDetailedText(message_2)
        self.msg.setStandardButtons(QMessageBox.StandardButton.Ok)
        returnValue = self.msg.exec()
    def displayMessage2(self, message_1):
        self.msg.setIcon(QMessageBox.Icon.Information)
        self.msg.setWindowTitle("Message")
        self.msg.setText(message_1)
        self.msg.setStandardButtons(QMessageBox.StandardButton.Ok)
        returnValue = self.msg.exec()
    def readFile(self):
        NoOfItem =self.senList.count()
        for ic in range(0,NoOfItem):
            self.senList.takeItem(0)
            self.YCols.takeItem(ic,2);
            self.YCols.takeItem(ic,1);
            self.YCols.takeItem(ic,0);
        self.YCols.setRowCount(0)
        if self.fileName and path.exists(self.fileName):
            self.scriptBtn.setEnabled(1)
            self.ext = self.fileName[self.fileName.rindex('.'):]
            self.plotObject_1 = [];
            if (self.ext == '.dat') | (self.ext == '.txt'):
                self.reader = np.loadtxt(self.fileName)
            if str(self.reader)=='[]':
                self.msg.setText("Output file has no data!")
                self.showWarningDialog();
            else:
                self.n_col = self.reader.shape[1]
                self.total_rows = self.reader.shape[0]
                if self.n_col == 0 or self.total_rows == 0:
                    self.msg.setText("Output file has no data!")
                    self.showWarningDialog();
                else:
                    fileI = self.DataFiles.index(self.fileName[self.fileName.rindex('/')+1:])
                    self.fileVariables=self.variables[fileI]
                    if len(self.fileVariables) < self.n_col:
                        self.fileVariables.insert(0,'time')
                    self.YCols.setRowCount(self.n_col)

                    self.powerPopup_1.combo_voltage.clear()
                    self.powerPopup_1.combo_current.clear()
                    for k in range(1, len(self.fileVariables)):
                        var_name = self.fileVariables[k]
                        self.powerPopup_1.combo_voltage.addItem(var_name)
                        self.powerPopup_1.combo_current.addItem(var_name)

                    self.YCols.setVerticalHeaderLabels(list([None]*self.n_col))
                    self.FlagLR = []
                    for cln in range(0,self.n_col):
                        if (self.ext == '.dat') | (self.ext == '.txt') | (self.header == 0):
                            cl = self.fileVariables[cln]
                        self.FlagLR.append('N')
                        self.plotObject_1.append(PlotObject(label=cl))
                        itm =QListWidgetItem()
                        itm.setText(cl)
                        itm.setFlags(itm.flags() | QtCore.Qt.ItemFlag.ItemIsUserCheckable)
                        itm.setCheckState(QtCore.Qt.CheckState.Unchecked)
                        self.senList.addItem(itm)
                        pixmap = QPixmap(10,10);
                        pixmap.fill(QColor("white"));
                        self.redIcon= QIcon(pixmap);
                        itm =  QTableWidgetItem("")
                        itm.setFlags(itm.flags() | QtCore.Qt.ItemFlag.ItemIsUserCheckable)
                        itm.setCheckState(QtCore.Qt.CheckState.Unchecked)
                        itm.setIcon(self.redIcon)
                        self.YCols.setItem(cln,0,itm)
                        itm =  QTableWidgetItem("")
                        itm.setFlags(itm.flags() | QtCore.Qt.ItemFlag.ItemIsUserCheckable)
                        itm.setCheckState(QtCore.Qt.CheckState.Unchecked)
                        itm.setIcon(self.redIcon)
                        self.YCols.setItem(cln,1,itm)
                        self.YCols.verticalHeader().setSectionResizeMode(cln, QHeaderView.ResizeMode.ResizeToContents)
                        itm =  QTableWidgetItem(cl)
                        self.YCols.setItem(cln,2,itm)
                        self.YCols.verticalHeader().setSectionResizeMode(cln, QHeaderView.ResizeMode.ResizeToContents)

                texC = "Columns: " + str(self.n_col)
                texR = "Rows: " + str(self.total_rows)

                self.label.setText(texC)
                self.label.setEnabled(1)
                self.labelR.setText(texR)
                self.labelR.setEnabled(1)
                if (self.n_col) >=2:
                    self.XIndex = 0;
                    self.YIndex = [];
                    self.YIndexR = [];
                    self.senList.item(0).setCheckState(QtCore.Qt.CheckState.Checked)
                    self.pltBtn.setEnabled(1)

class PlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=3, dpi=100):
        super().__init__()
        self.fig = Figure(figsize=(width, height))
        FigureCanvas.__init__(self, self.fig)
        self.setParent(parent)
        FigureCanvas.setSizePolicy(self, QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)

        FigureCanvas.setGeometry(self,QRect(695,200,700,450))
        FigureCanvas.updateGeometry(self)

        self.ax_multi = [None]

        self.ax_power  = [None]
        self.ax2_power = [None]

        self.plot()

    def plot(self):

        self.ax = self.fig.add_subplot(111)
        self.ax.grid()
        self.ax.set_xlabel("X-axis")
        self.ax.set_ylabel("Y-axis")
        self.ax2 = self.ax.twinx()
        self.draw()
        return

    def plotData(self,x,y,pltColor='red',lineStyle='solid',lineWidth=0.7,drawStyle="default",
        pltLabel = '',markerStyle ='',markerSize = 7, markerEdgeColor = 'black',
        markerFaceColor='red',yax='left'):

        if yax =='right':
            pl=self.ax2.plot(x,y, color = pltColor,linestyle=lineStyle,
                linewidth=lineWidth,drawstyle=drawStyle,
                label=pltLabel, marker = markerStyle, markersize = markerSize,
                markeredgecolor = markerEdgeColor,markerfacecolor = markerFaceColor)
            self.ax.autoscale(enable=True,axis='x',tight=True)
            self.draw()
        else:
            pl=self.ax.plot(x,y, color = pltColor,linestyle=lineStyle,
                linewidth=lineWidth,drawstyle=drawStyle,
                label=pltLabel, marker = markerStyle, markersize = markerSize,
                markeredgecolor = markerEdgeColor,markerfacecolor = markerFaceColor)
            self.ax.autoscale(enable=True,axis='x',tight=True)
            self.draw()

        return pl;
    def showLegend(self):
        ax = self.fig.axes
        ax = ax[0]
        if len(mainWin.YIndex) > 0 and len(mainWin.YIndexR) == 0:
            self.ax.legend(loc = mainWin.legendObject_1.location,frameon = mainWin.legendObject_1.frameon,
                fontsize = mainWin.legendObject_1.fontsize, title = mainWin.legendObject_1.title,
                markerfirst = mainWin.legendObject_1.markerfirst, markerscale = mainWin.legendObject_1.markerscale,
                labelspacing = mainWin.legendObject_1.labelspacing, columnspacing = mainWin.legendObject_1.columnspacing)
        elif len(mainWin.YIndexR)>0 and len(mainWin.YIndex) == 0:
            self.ax2.legend(loc = mainWin.legendObject_1.location,frameon = mainWin.legendObject_1.frameon,
                fontsize = mainWin.legendObject_1.fontsize, title = mainWin.legendObject_1.title,
                markerfirst = mainWin.legendObject_1.markerfirst, markerscale = mainWin.legendObject_1.markerscale,
                labelspacing = mainWin.legendObject_1.labelspacing, columnspacing = mainWin.legendObject_1.columnspacing)
        elif len(mainWin.YIndex) > 0 and len(mainWin.YIndexR) > 0:
            lns = mainWin.pls[0]
            for l in range(1,len(mainWin.pls)):
                lns = lns+mainWin.pls[l]
            self.ax.legend(lns,mainWin.plbs)

        self.draw()

    def removeLegend(self):
        ax = self.fig.axes
        ax = ax[0]
        ax.get_legend().remove()
        self.draw()

    def changeAxesProps(self):
        ax = self.fig.axes
        ax = ax[0]
        if mainWin.axesPropObject_1.xScale != None:
            self.ax.set_xscale(mainWin.axesPropObject_1.xScale)
        if mainWin.axesPropObject_1.yScale != None and len(mainWin.YIndex)!=0:
            self.ax.set_yscale(mainWin.axesPropObject_1.yScale)
        else:
            self.ax.set_yscale(mainWin.axesPropObject_1.yScale2)
        if mainWin.axesPropObject_1.yScale2 != None and len(mainWin.YIndexR)!=0:
            self.ax2.set_yscale(mainWin.axesPropObject_1.yScale2)
        else:
            self.ax2.set_yscale(mainWin.axesPropObject_1.yScale)

        if mainWin.axesPropObject_1.xLabel != None:
            self.ax.set_xlabel(mainWin.axesPropObject_1.xLabel)

        if mainWin.axesPropObject_1.yLabel != None and len(mainWin.YIndex)!=0:
            self.ax.set_ylabel(mainWin.axesPropObject_1.yLabel)
        else:
            self.ax.set_ylabel(mainWin.axesPropObject_1.yLabel2)

        if mainWin.axesPropObject_1.yLabel2 != None and len(mainWin.YIndexR)!=0:
            self.ax2.set_ylabel(mainWin.axesPropObject_1.yLabel2)
        else:
            self.ax2.set_ylabel(mainWin.axesPropObject_1.yLabel)

        if (mainWin.axesPropObject_1.s_xMin.split()):
            self.ax.set_xlim(left = float(mainWin.axesPropObject_1.s_xMin))

        if (mainWin.axesPropObject_1.s_xMax.split()):
            self.ax.set_xlim(right = float(mainWin.axesPropObject_1.s_xMax))

        if len(mainWin.YIndex) != 0:
            if (mainWin.axesPropObject_1.s_yMin.split()):
                self.ax.set_ylim(bottom = float(mainWin.axesPropObject_1.s_yMin))
            if (mainWin.axesPropObject_1.s_yMax.split()):
                self.ax.set_ylim(top = float(mainWin.axesPropObject_1.s_yMax))

        if len(mainWin.YIndexR) != 0:
            if (mainWin.axesPropObject_1.s_yMin2.split()):
                self.ax2.set_ylim(bottom = float(mainWin.axesPropObject_1.s_yMin2))
            if (mainWin.axesPropObject_1.s_yMax2.split()):
                self.ax2.set_ylim(top = float(mainWin.axesPropObject_1.s_yMax2))

        if (len(mainWin.YIndex) != 0) and (len(mainWin.YIndexR) == 0):
            y_min, y_max = self.ax.get_ylim()
            self.ax2.set_ylim(bottom=y_min, top=y_max)
        if (len(mainWin.YIndex) == 0) and (len(mainWin.YIndexR) != 0):
            y_min, y_max = self.ax2.get_ylim()
            self.ax.set_ylim(bottom=y_min, top=y_max)

        if mainWin.axesPropObject_1.xScale == 'linear':
            self.ax.ticklabel_format(axis="x", style="sci", scilimits=(mainWin.axesPropObject_1.xSNL,\
                    mainWin.axesPropObject_1.xSNU),useMathText=mainWin.axesPropObject_1.xSNM)
        if mainWin.axesPropObject_1.yScale == 'linear' and len(mainWin.YIndex)!=0:
            self.ax.ticklabel_format(axis="y", style="sci", scilimits=(mainWin.axesPropObject_1.ySNL,\
            mainWin.axesPropObject_1.ySNU),useMathText=mainWin.axesPropObject_1.ySNM)
        else:
            self.ax.ticklabel_format(axis="y", style="sci", scilimits=(mainWin.axesPropObject_1.ySNL2,\
            mainWin.axesPropObject_1.ySNU2),useMathText=mainWin.axesPropObject_1.ySNM2)
        if mainWin.axesPropObject_1.yScale2 == 'linear' and len(mainWin.YIndexR)!=0:
            self.ax2.ticklabel_format(axis="y", style="sci", scilimits=(mainWin.axesPropObject_1.ySNL2,\
            mainWin.axesPropObject_1.ySNU2),useMathText=mainWin.axesPropObject_1.ySNM2)
        else:
            self.ax2.ticklabel_format(axis="y", style="sci", scilimits=(mainWin.axesPropObject_1.ySNL,\
            mainWin.axesPropObject_1.ySNU),useMathText=mainWin.axesPropObject_1.ySNM)

        self.draw()

    def getAxesProps(self):
        xMin, xMax = self.ax.get_xlim()
        yMin, yMax = self.ax.get_ylim()
        yMin2, yMax2 = self.ax2.get_ylim()
        mainWin.axesPropObject_1.setxMin(xMin);
        mainWin.axesPropObject_1.setxMax(xMax);
        mainWin.axesPropObject_1.setyMin(yMin);
        mainWin.axesPropObject_1.setyMax(yMax);
        mainWin.axesPropObject_1.setyMin2(yMin2);
        mainWin.axesPropObject_1.setyMax2(yMax2);

    def changeGridProps(self):
        if mainWin.gridObject_1.gridEnable:
            self.ax.grid(b=True,color = mainWin.gridObject_1.lineColor,
                    linestyle = mainWin.gridObject_1.lineStyle,
                    axis = mainWin.gridObject_1.axis,
                    which = mainWin.gridObject_1.which,
                    linewidth = mainWin.gridObject_1.width)
        else:
            self.ax.grid(b=None)

        self.draw()

    def changeTitle(self):
        if mainWin.titleObject_1.titleEnable:
            self.ax.set_title(label='')
            self.ax.set_title(label='',loc='left')
            self.ax.set_title(label='', loc='right')
            self.ax.set_title(label=mainWin.titleObject_1.label,loc=mainWin.titleObject_1.loc)
        else:
            self.ax.set_title(label='')

        self.draw()

    def getxTicks(self):
        mainWin.ticksPropObject_x.setXTicks(list(self.ax.get_xticks()))
        mainWin.ticksPropObject_y1.setXTicks(list(self.ax.get_yticks()))
        mainWin.ticksPropObject_y2.setXTicks(list(self.ax2.get_yticks()))
        bn = []
        for tick in self.ax.get_xticklabels():
            bn+= [str(tick.get_text())]

        mainWin.ticksPropObject_x.setXTicksLabels(bn)
        bn = []
        for tick in self.ax.get_yticklabels():
            bn+= [str(tick.get_text())]
        mainWin.ticksPropObject_y1.setXTicksLabels(bn)

        bn = []
        for tick in self.ax2.get_yticklabels():
            bn+= [str(tick.get_text())]
        mainWin.ticksPropObject_y2.setXTicksLabels(bn)
    def changeXTicks(self):
        if mainWin.ticksPropObject_x.xTicksEnable:
            self.ax.set_xticks(mainWin.ticksPropObject_x.xticks)
            self.ax.tick_params(axis='x',direction=mainWin.ticksPropObject_x.direction)
        else:
            self.ax.set_xticks([])
        self.draw()
    def changeXTicksLabels(self):
        if mainWin.ticksPropObject_x.xTicksEnable:
            self.ax.set_xticks(mainWin.ticksPropObject_x.xticks)
            self.ax.tick_params(axis='x',direction=mainWin.ticksPropObject_x.direction)
            self.ax.set_xticklabels(mainWin.ticksPropObject_x.xtickslabels,rotation = mainWin.ticksPropObject_x.rotation)
        else:
            self.ax.set_xticks([])
        self.draw()
    def changeYTicks(self):
        if mainWin.ticksPropObject_y1.xTicksEnable:
            self.ax.set_yticks(mainWin.ticksPropObject_y1.xticks)
            self.ax.tick_params(axis='y',direction=mainWin.ticksPropObject_y1.direction)
        else:
            self.ax.set_yticks([])
        self.draw()
    def changeYTicksLabels(self):
        if mainWin.ticksPropObject_y1.xTicksEnable:
            self.ax.set_yticks(mainWin.ticksPropObject_y1.xticks)
            self.ax.tick_params(axis='y',direction=mainWin.ticksPropObject_y1.direction)
            self.ax.set_yticklabels(mainWin.ticksPropObject_y1.xtickslabels,rotation = mainWin.ticksPropObject_y1.rotation)
        else:
            self.ax.set_yticks([])
        self.draw()
    def changeYTicks2(self):
        if mainWin.ticksPropObject_y2.xTicksEnable:
            self.ax2.set_yticks(mainWin.ticksPropObject_y2.xticks)
            self.ax2.tick_params(axis='y',direction=mainWin.ticksPropObject_y2.direction)
        else:
            self.ax.set_yticks([])
        self.draw()
    def changeYTicksLabels2(self):
        if mainWin.ticksPropObject_y2.xTicksEnable:
            self.ax2.set_yticks(mainWin.ticksPropObject_y2.xticks)
            self.ax2.tick_params(axis='y',direction=mainWin.ticksPropObject_y2.direction)
            self.ax2.set_yticklabels(mainWin.ticksPropObject_y2.xtickslabels,
                rotation = mainWin.ticksPropObject_y2.rotation)
        else:
            self.ax2.set_yticks([])
        self.draw()

class PlotWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('Plot Data')

        self.canvas = PlotCanvas()
        self.fig = self.canvas.fig

        self.setCentralWidget(self.canvas)

if __name__ == "__main__":
    # QGuiApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling, True);
    app = QtWidgets.QApplication([])

#   Note: include / at the end in passing dir_output_files
    if len(sys.argv) != 3:
        print('plot_main: need 3 arguments. Halting...')
        sys.exit()
    dir_output_files = sys.argv[1]
    file_last_opened = sys.argv[2]

    mainWin = ApplicationWindow(dir_output_files, file_last_opened)

    mainWin.show()
    sys.exit( app.exec())
