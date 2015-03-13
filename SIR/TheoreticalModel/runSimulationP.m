
clear all; close all; clc;
tend  = 200;
dt    = 0.1;
ic    = [0.9995,1-0.9995,0];

%% === SIRDE(tend, dt, ic)=======================
SIR1 = SIRDE(tend,0.2,ic, 2000);

% ===== V26 =======
SIR1.beta  = 6.75;
SIR1.gamma = 3;
SIR1.mu    = .051; 


SIR1.Simulate(1);
%SIR1.plot('LegendOn')
%grid on

SIR1.getEigen();
SIR1.singlePlot('All',1)

%SIR1.makeMovie(3)

%open('../SIRXCode/SIR/Results/res_3-2-2015/sim_v26.fig')
%set(gcf,'units','normalized','outerposition',[0 0 1 1])
%hold on
%SIR1.plot('LegendOn')

