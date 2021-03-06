classdef Visualization < handle
    
    properties
        HistoryData;
        MovieData;
        ShowHistory;
        ShowMovie;
        Showgraph;
        Frames;
        CityBoundary;
        City;
        Homes   = {};
        Schools = {};
        Works   = {};
        Cemeteries  = {};
        polyxs  = [];
        polyys  = [];
        colors  = [];
        People  = {};
        Peopx   = [];
        Peopy   = [];
        MovieFolder;
        DataFolder;
        MaxInfLev = 0.01;
        FullScreen = 0;
        ShowNodes  = 0;
        MakeMovie  = 0;
        graphFiles;
        Adj;
        Nodes;
        GraphType = 'SI';
        TaggedPeople = [];
    end
    
    methods
        function v = Visualization(dataFolder, movieFolder, showHistory, showMovie, showGraph)
            v.MovieFolder = movieFolder;
            v.DataFolder = dataFolder;
            
            if showHistory
                historyFile = [dataFolder, '/historyData.dat'];
                v.ReadHistoryData(historyFile);
            end
            if showMovie
                cityFile = [dataFolder, '/cityData.dat'];
                homeFile = [dataFolder, '/homeData.dat'];
                workFile = [dataFolder, '/workData.dat'];
                schoolFile = [dataFolder, '/schoolData.dat'];
                cemeteryFile = [dataFolder, '/cemeteryData.dat'];
                v.ReadCityData(cityFile);
                v.ReadHomeData(homeFile);
                v.ReadSchoolData(schoolFile);
                v.ReadWorkData(workFile)
                v.ReadCemeteryData(cemeteryFile)
            end
            if showGraph
                directoryfiles = dir(['../Data/', movieFolder]);
                
                files = cell(1, length(directoryfiles)-2);
                for ii = 1:length(files)
                    files{ii} = directoryfiles(ii + 2).name;
                end
               v.graphFiles = files;
            end
        end
        function ReadHistoryData(obj, historyFile)
            obj.HistoryData =  readtable(historyFile);
        end
        function ReadCityData(obj, cityFile)
            city =  readtable(cityFile);
            obj.City.Name = city.Name;
            obj.City.Boundary  = [city.xmin, city.xmax; city.ymin, city.ymax];
        end
        function ReadHomeData(obj, homeFile)
            file = homeFile;
            home = readtable(file);
            obj.Homes = cell(1,length(home.Name));
            for ii = 1:length(obj.Homes)
                obj.Homes{ii}.Name = home.Name{ii};
                obj.Homes{ii}.Type = home.Type{ii};
                obj.Homes{ii}.Perimeter = [home.xmin(ii), home.xmax(ii); home.ymin(ii), home.ymax(ii)];
            end
        end
        function ReadSchoolData(obj, schoolFile)
            file = schoolFile;
            school = readtable(file);
            obj.Schools = cell(1,length(school.Name));
            for ii = 1:length(obj.Schools)
                obj.Schools{ii}.Name = school.Name{ii};
                obj.Schools{ii}.Type = school.Type{ii};
                obj.Schools{ii}.Perimeter = ...
                    [school.xmin(ii), school.xmax(ii); school.ymin(ii), school.ymax(ii)];
            end
        end
        function ReadCemeteryData(obj, cemeteryFile)
            file = cemeteryFile;
            cemetery = readtable(file);
            obj.Cemeteries = cell(1,length(cemetery.Name));
            for ii = 1:length(obj.Cemeteries)
                obj.Cemeteries{ii}.Name = cemetery.Name{ii};
                obj.Cemeteries{ii}.Type = cemetery.Type{ii};
                obj.Cemeteries{ii}.Perimeter = ...
                    [cemetery.xmin(ii), cemetery.xmax(ii); cemetery.ymin(ii), cemetery.ymax(ii)];
            end
        end
        function ReadWorkData(obj, workFile)
            file = workFile;
            work = readtable(file);
            obj.Works = cell(1,length(work.Name));
            for ii = 1:length(obj.Works)
                obj.Works{ii}.Name = work.Name{ii};
                obj.Works{ii}.Type = work.Type{ii};
                obj.Works{ii}.Perimeter = ...
                    [work.xmin(ii), work.xmax(ii); work.ymin(ii), work.ymax(ii)];
            end
        end
        function [A,C] = ProduceGraphData(obj, t, type)
            f = strcat(obj.MovieFolder,'/',obj.graphFiles(t));
            rawData = readtable(f{1});
            ids  = rawData.ID+1;
            xs   = rawData.x;
            ys   = rawData.y;
            switch type
                case 'SI'
                    cons = rawData.SIConnections;
                case 'SIHist'
                    cons = rawData.SIConnectionsHist;
                case 'All'
                    cons = rawData.AllConnections;
                otherwise 
                    cons = rawData.AllConnectionsHist;
            end
            A    = zeros(length(ids));
            C    = zeros(length(ids),2);
            
            for ii = 1:length(ids)
                c = eval(char(cons(ii)))+1;
                for jj = c
                    if ii == jj
                        A(ii,jj) = 0;
                    else
                        A(ii,jj) = 1;
                    end
                end
                C(ii,:) =[xs(ii), ys(ii)]; 
            end
        end
        function h = PlotHistory(obj, pop)
            t  = obj.HistoryData.Time;
            II = obj.HistoryData.Infected;
            PP = obj.HistoryData.Symptomatic;
            SS = obj.HistoryData.Suseptible;
            RR = obj.HistoryData.Recovered;
            DD = obj.HistoryData.Dead;
            total = II + PP + SS + RR + DD;
            h = figure;
            if obj.FullScreen
                set(gcf,'units','normalized','outerposition',[0 0 1 1])
            end
            if strcmp(pop,'SIRONLY')
                plot(t, SS,'b',...
                    t, PP+II,'r',...
                    t, RR,'g','linewidth',3);
                l = legend('Susceptible','Infected','Recovered');
            else
                plot(t, SS,'b',...
                    t, II,'y',...
                    t, PP,'r',...
                    t, RR,'g',...
                    t, DD,'k',...
                    'linewidth',3)
                l = legend('Susceptible','Infected','Symptomatic','Recovered','Dead');
            end
            
            set(l,'FontSize',18)
            xlabel('Time ($hr$)','interpreter','latex','FontSize',18);
            ylabel('Population','interpreter','latex','FontSize',18);
            grid on
        end
        function h = PlotPhase(obj)
            t  = obj.HistoryData.Time;
            II = obj.HistoryData.Infected;
            PP = obj.HistoryData.Symptomatic;
            SS = obj.HistoryData.Suseptible;
            RR = obj.HistoryData.Recovered;
            DD = obj.HistoryData.Dead;
            h = figure;
            if obj.FullScreen
                set(gcf,'units','normalized','outerposition',[0 0 1 1])
            end
            
            subplot(2,2,1)
            plot(II,SS,'k','linewidth',3);
            xlabel('Infected','interpreter','latex','FontSize',18);
            ylabel('Susceptible','interpreter','latex','FontSize',18);
            grid on
            
            subplot(2,2,2)
            plot(RR,SS,'k','linewidth',3);  
            xlabel('Recovered','interpreter','latex','FontSize',18);
            ylabel('Susceptible','interpreter','latex','FontSize',18);
            grid on
            
            subplot(2,2,3)
            plot(II,RR,'k','linewidth',3); 
            xlabel('Infected','interpreter','latex','FontSize',18);
            ylabel('Recovered','interpreter','latex','FontSize',18);
            grid on
            
            subplot(2,2,4)
            plot(RR,DD,'k','linewidth',3);
            xlabel('Recovered','interpreter','latex','FontSize',18);
            ylabel('Dead','interpreter','latex','FontSize',18);
            grid on
            
        end      
        function DrawCity(obj)
            A = obj.City.Boundary;
            obj.polyxs = [obj.polyxs,[A(1,1);A(1,2);A(1,2);A(1,1)]];
            obj.polyys = [obj.polyys,[A(2,1);A(2,1);A(2,2);A(2,2)]];
            obj.colors = [obj.colors;[0.8,1,.85]];
        end
        function DrawPlace(obj,place)
            if strcmpi(place.Type,'Home')
                c = [.7,.8,.8];
            elseif strcmpi(place.Type,'School')
                c = [.6,.7,.7];
            elseif strcmpi(place.Type,'Work')
                c = [.6,.6,.1];
            else % Cemetery
                c= [0.9,0.9,0.9];
            end

            A = place.Perimeter;
            obj.polyxs = [obj.polyxs,[A(1,1);A(1,2);A(1,2);A(1,1)]];
            obj.polyys = [obj.polyys,[A(2,1);A(2,1);A(2,2);A(2,2)]];
            obj.colors = [obj.colors;c];
        end        
        function DrawPeople(obj,person) 
            if strcmpi(person.State,'S')
                plot(person.Coordinates(1),person.Coordinates(2),'.b', 'MarkerSize',20)
            elseif strcmpi(person.State,'I')
                plot(person.Coordinates(1),person.Coordinates(2),'.y', 'MarkerSize',20)
            elseif strcmpi(person.State,'P')
                plot(person.Coordinates(1),person.Coordinates(2),'.r', 'MarkerSize',20)
            elseif strcmpi(person.State,'R')
                plot(person.Coordinates(1),person.Coordinates(2),'.g', 'MarkerSize',20)
            else % Dead people
                plot(person.Coordinates(1),person.Coordinates(2),'.k', 'MarkerSize',20)
            end
            if any(person.ID == obj.TaggedPeople)
                text(person.Coordinates(1),person.Coordinates(2),['\bf',int2str(person.ID)], 'fontsize', 18, 'Color', 'm')
            end
        end
        function DrawPeople_InHost(obj,person, t)
            b = abs(person.InfLev(t));
            hbs = person.HasBeenSick(t);
            cval = b/obj.MaxInfLev;
            if isnan(cval)
                cval = 0;
            end
            if cval > 1
                cval = 1;
            end
            if cval < 0
                cval = 0;
            end
            if hbs
                color = [cval,1-cval, 0];
            else
                color = [cval,0,1-cval];
            end
            p = plot(person.Coordinates(1),person.Coordinates(2), '.', 'MarkerSize',20);
            set(p,'Color', color);
        end
        function h = Render(obj,t)
            h = figure(1);
            if obj.FullScreen
                set(gcf,'units','normalized','outerposition',[0 0 1 1])
            end
            hold on
            obj.DrawCity();
            hold on
            for ii = 1:length(obj.Homes)
                obj.DrawPlace(obj.Homes{ii});
            end
            for ii = 1:length(obj.Works)
                obj.DrawPlace(obj.Works{ii});
            end
            for ii = 1:length(obj.Schools)
                obj.DrawPlace(obj.Schools{ii});
            end
            for ii = 1:length(obj.Cemeteries)
                obj.DrawPlace(obj.Cemeteries{ii});
            end
            p = patch(obj.polyxs,obj.polyys,'b'); hold on
            set(p,'FaceColor','flat','FaceVertexCData',obj.colors)
            hold on
            obj.DrawGraph(t,1,0.01);
            for ii = 1:length(obj.People)
                obj.DrawPeople_InHost(obj.People{ii}, t)
                hold on
            end
            
            title(sprintf('Time = %d',obj.People{1}.Time),'fontsize',22)
            
            day = floor(obj.People{1}.Time/24);
            mhour = mod(obj.People{1}.Time,24);
            hour = mod(obj.People{1}.Time,12);
             if hour == 0
                 hour = 12;
             end
             
             if mhour < 12
                 AmPm = 'am';
             else
                 AmPm = 'pm';
             end
            text(50,1870,sprintf('Day: %d',day),'FontSize',16)
            text(50,1750,[sprintf('Time: %d:00 ',hour),AmPm],'FontSize',16)
            
        end
        function ReadData(obj)
            directoryfiles = dir(['../Data/', obj.MovieFolder]);
            files = cell(1, length(directoryfiles)-2);
            for ii = 1:length(files)
                files{ii} = directoryfiles(ii + 2).name;
            end
            loops  = length(files);
            
            people = readtable([obj.MovieFolder,'/',files{end}]);
            obj.MaxInfLev = max(people.MaxInfLev);
            for jj = 1:length(people.ID)
                obj.People = cell(1,length(people.ID));
                obj.People{jj}.InfLev = zeros(1,loops);
                obj.People{jj}.SusLev = zeros(1,loops);
                obj.People{jj}.VirLev = zeros(1,loops);
            end
            wbh = waitbar(0,'Reading all the data. Please wait...');
            
            if obj.MakeMovie
                frames(loops) = struct('cdata',[],'colormap',[]);
                wbhm = waitbar(0,'Making a movie. Please wait...');
            end
           
            
            for ii = 1:loops
                people = readtable([obj.MovieFolder,'/',files{ii}]);
                
                
                for jj = 1:length(people.ID)
                    
                    obj.People{jj}.ID = people.ID(jj);
                    obj.People{jj}.Name = people.Name(jj);
                    obj.People{jj}.Time = people.Time(jj);
                    obj.People{jj}.Coordinates = [people.x(jj), people.y(jj)];
                    obj.People{jj}.Location = people.Location(jj);
                    obj.People{jj}.State = people.State(jj);
                    obj.People{jj}.InfLev(ii) = people.InfectionLevel(jj);
                    obj.People{jj}.SusLev(ii) = people.SusceptibleCells(jj);
                    obj.People{jj}.VirLev(ii) = people.VirionLevel(jj);
                    obj.People{jj}.HasBeenSick(ii) = people.HasBeenSick(ii);
           
                end
                
                waitbar(ii/loops,wbh,sprintf('%2.1f %% of this step complete...',100 * ii/loops))
                
                if obj.MakeMovie
                    h = obj.Render(ii);
                    frames(ii) = getframe(h);
                    
                    waitbar(ii/loops,wbhm,sprintf('%2.1f %% of this step complete...',100 * ii/loops))
                end
                
            end
            close(wbh)
            if obj.MakeMovie
                close(wbhm)
            
                obj.Frames = frames;
            end
        end
        function h = DrawGraph(obj, t, fignum, pad)
            [A,C] = obj.ProduceGraphData(t, obj.GraphType);  
            obj.Adj = sparse(A);
            obj.Nodes = C;
            figure(fignum)
            gplot(obj.Adj,obj.Nodes,'k-*');
            
            if obj.ShowNodes 
               [fn,sn,~] = find(obj.Adj);
                 nodes = [fn;sn];
                 for n = nodes
                     text(obj.Nodes(n,1),obj.Nodes(n,2)+pad,num2str(n-1,'%d'),'fontsize', 18)
                 end
            end
        end
        function h = PlotIndividual(obj,fignum, pltnum, ppl)
           
            h = figure(fignum);
            if obj.FullScreen
                set(gcf,'units','normalized','outerposition',[0 0 1 1])
            end
            
            ymax = 0;
            
            for ii = 1:pltnum
                p = obj.People{ppl(ii)};
                S = p.SusLev;
                I = p.InfLev;
                V = p.VirLev;
                ymax = max(ymax,max([max(S),max(I), max(V)]));
            end
            
            for ii = 1:pltnum
                p = obj.People{ppl(ii)};
                S = p.SusLev;
                I = p.InfLev;
                V = p.VirLev;
                t = 1:length(S);
                subplot(pltnum,1,ii)
                plot(t, S, 'b', t, I, 'r', t, V, 'g', 'linewidth', 3)
                ylabel(sprintf('ID: %d',ppl(ii)),'FontSize', 16);
                grid on
                ylim([0, ymax])
            end
            %subplot(pltnum,1,1)
            l = legend ('Susceptible cells','Infected Cells', 'Free Virion');
            newPosition = [0.31 0.6 0.4 0.73];
            newUnits = 'normalized';
            set(l,'Position', newPosition,'Units', newUnits, 'Orientation','horizontal', 'FontSize', 18);
            %set(l, 'FontSize', 18, 'Location', 'NorthOutside','Orientation','horizontal')
            
            subplot(pltnum,1,pltnum)
            xlabel('Time', 'FontSize', 16)
        end
    end
end
