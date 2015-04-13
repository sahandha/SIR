classdef EigenData < handle
    
    properties
        Dim  = 1;
        D    = [];
        numD = [];
        x    = [];
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
            ed.D    = zeros(dim, length(ed.t));
            ed.numD = zeros(dim, length(ed.t));
            ed.file = f;
            ed.readEigenData(ed.file);
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
        end
        function setEigenData(obj,DD)
            obj.compareq = 1;
            obj.D = DD;
        end
        function EigenHistory(obj)
            for ii = 1:length(obj.t)
                dfdx = obj.njacobian(obj.x(ii), obj.xbar(ii), obj.xj(:,:,ii), obj.xbarj(:,:,ii), obj.dt);
                Df = dfdx;
                [~, D1] = eig(Df);
                obj.numD(:,ii) = diag(D1);
            end
        end
        function dfdx = njacobian(~, x, xbar, xj, xbarj, dt)
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
            dx = diag(xj) - x;
            
            
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
                subplot(obj.Dim,1)
                plot(obj.t,obj.xx(ii,:),'linewidth',5)
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
                else
                    plot(obj.t, real(obj.numD(ii,:)),'k','linewidth',5)                    
                end
                grid on
                xlabel('Time $t$','Interpreter','Latex','FontSize',18)
                ylabel(['$\lambda',sprintf('_%d',ii),'$'],'Interpreter','Latex','FontSize',18)
                
                subplot(obj.Dim,2,2*ii)
                if obj.compareq
                    plot(obj.t, imag(obj.D(ii,:)), obj.t, imag(obj.numD(ii,:)),'k-.','linewidth',5)
                    l = legend('Jacobian', 'Numerical $Df$');
                    set(l, 'FontSize',20, 'Interpreter', 'Latex')
                else
                    plot(obj.t, imag(obj.numD(ii,:)),'k','linewidth',5)
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
        
    end    
end