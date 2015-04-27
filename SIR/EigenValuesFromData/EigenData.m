classdef EigenData < handle
    
    properties
        Dim  = 1;
        D    = [];
        numD = [];
        x    = [];
        dx   = [];
        xbar = [];
        xj   = [];
        xbarj= [];
        t    = [];
        dt   = 0.1;
        compareq = 0;
        xdata;
        file;
    end
    
    methods
        function ed = EigenData(dim, f)
            ed.Dim  = dim;
            ed.file = f;
            ed.readEigenData(ed.file);
            ed.D    = zeros(dim, length(ed.t));
            ed.numD = zeros(dim, length(ed.t));
        end
        function readEigenData(obj,xfile)
            obj.xdata = readtable(xfile);
            obj.t     = obj.xdata.t;
            obj.dt    = diff(obj.xdata.t(1:2));

            obj.x     = zeros(length(obj.t),obj.Dim);
            for ii = 1:obj.Dim
                obj.x(:,ii) = eval(sprintf('obj.xdata.x_%d',ii));
            end
            
            xxj    = zeros(length(obj.t),obj.Dim^2);
            for ii = 1:obj.Dim^2
                xxj(:,ii) = eval(sprintf('obj.xdata.xj_%d',ii));
            end
            obj.xj    = zeros(obj.Dim,obj.Dim,length(obj.t));
            for ii = 1:length(obj.t)
                for jj = 1:obj.Dim
                    obj.xj(jj,:,ii) = xxj(ii,obj.Dim*jj-(obj.Dim-1):obj.Dim*jj);
                end
            end
            
            obj.xbar = zeros(length(obj.t),obj.Dim);
            for ii = 1:obj.Dim
                obj.xbar(:,ii) = eval(sprintf('obj.xdata.xbar_%d',ii));
            end
            
            xxbarj    = zeros(length(obj.t),obj.Dim^2);
            for ii = 1:obj.Dim^2
                xxbarj(:,ii) = eval(sprintf('obj.xdata.xbarj_%d',ii));
            end
            obj.xbarj    = zeros(obj.Dim,obj.Dim,length(obj.t));
            for ii = 1:length(obj.t)
                for jj = 1:obj.Dim
                    obj.xbarj(jj,:,ii) = xxbarj(ii,obj.Dim*jj-(obj.Dim-1):obj.Dim*jj);
                end
            end 
            obj.dx = diag(obj.xj(:,:,1)) - obj.x(1,:)';
        end
        function setEigenData(obj,DD)
            obj.compareq = 1;
            obj.D = DD;
        end
        function EigenHistory(obj)
            for ii = 1:length(obj.t)
                dfdx = obj.njacobian(obj.x(ii,:)', obj.xbar(ii,:)', obj.xj(:,:,ii), obj.xbarj(:,:,ii), obj.dt, obj.dx);
                
                Df = dfdx;
                [~, D1] = eig(Df);
                obj.numD(:,ii) = diag(D1);
               
            end
        end
        function dfdx = njacobian(~, x, xbar, xj, xbarj, dt, dx)
            % ==================================================================
            % | dfdx = njacobian(x, xbar, xj, xbarj, dt)
            % | Compute Jacobian from Data only, for only one time step.
            % | - x is a column vector of state values at the current time.
            % | - xbar is a column vector of state values f(x) at time t + dt.
            % | - xj is an nxn matrix of x + hjej values at the current time.
            % | - xbarj is an nxn matrix of f(x + hjej) values at t + dt.
            % | - dt is the time increment.
            % =================================================================
            
            n = length(x);
            
            dfdx = zeros(n);
            
            for ii = 1:n
                for jj = 1:n
                    dfdx(ii,jj) = (xbarj(ii,jj) - xj(ii,jj) - xbar(ii) + x(ii))/(dt*dx(ii));
                end
            end
        end
        function PlotTrajectory(obj)
            figure
            for ii = 1:obj.Dim
                subplot(obj.Dim,1,ii)
                plot(obj.t,obj.x(:,ii),'linewidth',5)
                grid on
                xlabel('Time $t$','Interpreter','Latex','FontSize',18)
                ylabel('Space $x$','Interpreter','Latex','FontSize',15)
            end
        end
        function PlotEigenValues(obj)
            figure
            set(gcf,'units','normalized','outerposition',[0 0 1 1])
            for ii = 1:obj.Dim
                subplot(obj.Dim,2,2*ii-1)
                if obj.compareq
                    plot(obj.t, real(obj.D(ii,:)), obj.t, real(obj.numD(ii,:)),'k-.','linewidth',5)                    
                    l = legend('Jacobian', 'Numerical $Df$');
                    set(l, 'FontSize',20, 'Interpreter', 'Latex')
                    ylim([min(-1,min(real(obj.numD(ii,:)))),max(1,max(real(obj.numD(ii,:))))]);
                else
                    plot(obj.t, real(obj.numD(ii,:)),'k','linewidth',5)
                    ylim([min(-1,min(real(obj.numD(ii,:)))),max(1,max(real(obj.numD(ii,:))))]);
                end
                grid on
                xlabel('Time $t$','Interpreter','Latex','FontSize',18)
                ylabel(['$\lambda',sprintf('_%d',ii),'$'],'Interpreter','Latex','FontSize',18)
                
                subplot(obj.Dim,2,2*ii)
                if obj.compareq
                    plot(obj.t, imag(obj.D(ii,:)), obj.t, imag(obj.numD(ii,:)),'k-.','linewidth',5)
                    l = legend('Jacobian', 'Numerical $Df$');
                    set(l, 'FontSize',20, 'Interpreter', 'Latex')
                    ylim([min(-1,min(imag(obj.numD(ii,:)))),max(1,max(imag(obj.numD(ii,:))))]);
                else
                    plot(obj.t, imag(obj.numD(ii,:)),'k','linewidth',5)
                    ylim([min(-1,min(imag(obj.numD(ii,:)))),max(1,max(imag(obj.numD(ii,:))))]);
                end
                
                grid on
                
                xlabel('Time $t$','Interpreter','Latex','FontSize',18)
                ylabel(['$\lambda',sprintf('_%d',ii),'$'],'Interpreter','Latex','FontSize',18)
                subplot(obj.Dim,2, 1)
                title('Real Part','FontSize',20)
                subplot(obj.Dim,2, 2)
                title('Imaginary Part','FontSize',20)
                
            end
        end
        function argonPlotEigen(obj)
            lam1 = obj.numD(1,:);
            lam2 = obj.numD(2,:);
            lam3 = obj.numD(3,:);
            
            figure
            subplot(2,1,1)
            
            plot(real(lam1),imag(lam1),'k.')
            title('\lambda_1','FontSize',20)
            xlabel('Real Part','FontSize',18)
            ylabel('Imaginary Part','FontSize',18)
           
            subplot(2,1,2)
            plot(real(lam2),imag(lam2),'k.',...
                 real(lam3),imag(lam3),'b.')
            title('\lambda_2 and \lambda_3','FontSize',20)
            xlabel('Real Part','FontSize',18)
            ylabel('Imaginary Part','FontSize',18)
            l = legend('\lambda_2','\lambda_3');
            set(l,'FontSize',18)
        end
        
    end    
end