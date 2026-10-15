/*
 * SparkScope — motor de la simulación.
 * Contiene el estado del clúster simulado, el planificador, los fallos,
 * las métricas y las actualizaciones visuales de la interfaz.
 */

"use strict";
    // Controles que permiten modificar el tamaño y comportamiento del clúster.
    const input = {
      users: document.getElementById("users"), workers: document.getElementById("workers"),
      cores: document.getElementById("cores"), partitions: document.getElementById("partitions"),
      faultRate: document.getElementById("faultRate"), adaptive: document.getElementById("adaptive")
    };
    const labels = {
      users: document.getElementById("usersValue"), workers: document.getElementById("workersValue"),
      cores: document.getElementById("coresValue"), partitions: document.getElementById("partitionsValue"),
      faultRate: document.getElementById("faultValue")
    };
    // Estado central. Las tareas viven en cola o dentro de un worker.
    const state = {running:true, jobId:0, taskId:0, completed:0, failures:0, totalLatency:0, jobsReceived:0, scheduled:0, autoClock:0, workers:[], queue:[], history:[], lastDispatch:null};
    function settings(){return {users:+input.users.value,workerCount:+input.workers.value,cores:+input.cores.value,partitions:+input.partitions.value,faultRate:+input.faultRate.value,adaptive:input.adaptive.checked};}
    function random(min,max){return Math.floor(Math.random()*(max-min+1))+min;}
    function updateLabels(){labels.users.textContent=input.users.value;labels.workers.textContent=input.workers.value;labels.cores.textContent=input.cores.value;labels.partitions.textContent=input.partitions.value;labels.faultRate.textContent=input.faultRate.value+"%";}
    function worker(id){return {id:id,status:"up",recovery:0,tasks:[],durations:[],recentFailures:0,pulse:0};}
    // Si se reducen workers, sus tareas no se pierden: regresan a la cola.
    function syncWorkers(){const total=settings().workerCount;while(state.workers.length<total)state.workers.push(worker(state.workers.length+1));while(state.workers.length>total){const removed=state.workers.pop();removed.tasks.forEach(function(t){t.assignedTo=null;state.queue.unshift(t);});}}
    // Un job se divide en una tarea por cada partición seleccionada.
    function submitJob(){const cfg=settings(),id=++state.jobId;for(let i=0;i<cfg.partitions;i+=1)state.queue.push({id:++state.taskId,jobId:id,remaining:random(4,11),original:0,wait:0,assignedTo:null});state.jobsReceived+=1;state.scheduled+=cfg.partitions;}
    function freeWorkers(){const cfg=settings();return state.workers.filter(function(w){return w.status==="up"&&w.tasks.length<cfg.cores;});}
    // La puntuación representa una decisión adaptativa basada en observaciones previas.
    function score(w,cores){const avg=w.durations.length?w.durations.reduce(function(a,b){return a+b;},0)/w.durations.length:7;return w.tasks.length/cores*5+avg*.18+w.tasks.length*.14+w.recentFailures*1.4;}
    // Cuando una tarea se asigna, se registra el destino para alimentar el
    // mensaje visual y se activa un pulso sobre el worker receptor.
    function schedule(){const cfg=settings();let available=freeWorkers();while(state.queue.length&&available.length){let chosen=available[0];if(cfg.adaptive)chosen=available.reduce(function(best,candidate){return score(candidate,cfg.cores)<score(best,cfg.cores)?candidate:best;},available[0]);const task=state.queue.shift();task.assignedTo=chosen.id;task.original=task.remaining;chosen.tasks.push(task);chosen.pulse=2;state.lastDispatch={taskId:task.id,jobId:task.jobId,workerId:chosen.id};available=freeWorkers();}}
    function progress(){state.workers.forEach(function(w){w.pulse=Math.max(0,w.pulse-1);if(w.status!=="up")return;for(let i=w.tasks.length-1;i>=0;i-=1){const task=w.tasks[i];task.remaining-=1;if(task.remaining<=0){const latency=task.original+task.wait;state.completed+=1;state.totalLatency+=latency;w.durations.push(task.original);if(w.durations.length>10)w.durations.shift();w.tasks.splice(i,1);}}});state.queue.forEach(function(t){t.wait+=1;});}
    // Al fallar, las tareas activas vuelven a estar disponibles para reasignación.
    function failWorker(w){if(!w||w.status!=="up")return false;w.status="down";w.recovery=random(5,10);w.recentFailures+=1;state.failures+=1;w.tasks.forEach(function(t){t.assignedTo=null;t.remaining=Math.max(2,t.remaining);state.queue.unshift(t);});w.tasks=[];return true;}
    function recover(){state.workers.forEach(function(w){if(w.status==="down"){w.recovery-=1;if(w.recovery<=0){w.status="up";w.recentFailures=Math.max(0,w.recentFailures-1);}}});}
    function autoJob(){const cfg=settings();state.autoClock+=1;const interval=Math.max(3,19-Math.floor(cfg.users/2));if(state.autoClock>=interval){submitJob();state.autoClock=0;}}
    function randomFailure(){const cfg=settings();if(Math.random()<cfg.faultRate/1000){const candidates=state.workers.filter(function(w){return w.status==="up";});if(candidates.length)failWorker(candidates[random(0,candidates.length-1)]);}}
    function pct(value,total){return total?Math.min(100,Math.round(value*100/total)):0;}
    function record(){const cfg=settings(),capacity=state.workers.length*cfg.cores,active=state.workers.reduce(function(sum,w){return sum+w.tasks.length;},0);state.history.push({queue:state.queue.length,util:pct(active,capacity)});if(state.history.length>30)state.history.shift();}
    function renderWorkers(){const cfg=settings(),box=document.getElementById("workersContainer");box.innerHTML="";state.workers.forEach(function(w){const active=w.tasks.length,memory=Math.min(100,active*18+(w.status==="down"?0:6)),card=document.createElement("article");card.className="node worker"+(w.status==="down"?" down":"")+(w.pulse>0?" receiving":"");let tasks="";for(let i=0;i<cfg.cores;i+=1){const t=w.tasks[i];tasks+=t?'<div class="task" title="Job '+t.jobId+', tarea '+t.id+'">T'+t.id+"</div>":'<div class="task empty">·</div>';}const status=w.status==="down"?"RECUPERANDO "+w.recovery+"s":"ACTIVO";card.innerHTML='<div class="worker-top"><div><div class="type">Worker '+w.id+'</div><div class="name">Executor '+w.id+'</div></div><span class="badge">'+status+"</span></div>"+'<div class="bar-label"><span>Cores</span><span>'+active+" / "+cfg.cores+"</span></div><div class=\"bar\"><div style=\"width:"+pct(active,cfg.cores)+'%"></div></div><div class="bar-label"><span>Memoria simulada</span><span>'+memory+'%</span></div><div class="bar"><div style="width:'+memory+'%"></div></div><div class="tasks">'+tasks+"</div>";box.appendChild(card);});}
    function renderJobs(){const box=document.getElementById("jobs");if(!state.queue.length){box.innerHTML='<div class="empty-msg">No hay tareas esperando recursos.</div>';return;}const grouped={};state.queue.forEach(function(t){grouped[t.jobId]=(grouped[t.jobId]||0)+1;});box.innerHTML=Object.keys(grouped).slice(0,8).map(function(id){return '<div class="job"><b>Job '+id+"</b><br><span>"+grouped[id]+" tarea(s) pendientes</span></div>";}).join("");}
    function renderMetrics(){const cfg=settings(),capacity=state.workers.length*cfg.cores,active=state.workers.reduce(function(sum,w){return sum+w.tasks.length;},0),down=state.workers.filter(function(w){return w.status!=="up";}).length;document.getElementById("completed").textContent=state.completed;document.getElementById("queued").textContent=state.queue.length;document.getElementById("utilization").textContent=pct(active,capacity)+"%";document.getElementById("latency").textContent=(state.completed?state.totalLatency/state.completed:0).toFixed(1)+" s";document.getElementById("down").textContent=down;document.getElementById("failures").textContent=state.failures;document.getElementById("jobsReceived").textContent=state.jobsReceived;document.getElementById("scheduled").textContent=state.scheduled;document.getElementById("mode").textContent=cfg.adaptive?"Adaptativo":"Primero disponible";document.getElementById("status").textContent=state.running?"● Simulación en ejecución":"● Simulación pausada";}
    // Gráfica Canvas propia para que el archivo no dependa de una biblioteca o CDN.
    function renderChart(){const canvas=document.getElementById("chart"),rect=canvas.getBoundingClientRect(),ratio=window.devicePixelRatio||1,ctx=canvas.getContext("2d");canvas.width=Math.max(1,Math.floor(rect.width*ratio));canvas.height=Math.max(1,Math.floor(rect.height*ratio));ctx.scale(ratio,ratio);const w=rect.width,h=rect.height,p={l:33,r:12,t:14,b:27},data=state.history.length?state.history:[{queue:0,util:0}],maxQueue=Math.max(8,data.reduce(function(m,v){return Math.max(m,v.queue);},0));ctx.clearRect(0,0,w,h);ctx.strokeStyle="rgba(159,179,200,.18)";for(let i=0;i<4;i+=1){const y=p.t+i*(h-p.t-p.b)/3;ctx.beginPath();ctx.moveTo(p.l,y);ctx.lineTo(w-p.r,y);ctx.stroke();}function line(key,max,color){ctx.strokeStyle=color;ctx.lineWidth=2.5;ctx.beginPath();data.forEach(function(v,i){const x=p.l+i*(w-p.l-p.r)/Math.max(1,data.length-1),y=h-p.b-v[key]*(h-p.t-p.b)/max;if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);});ctx.stroke();}line("queue",maxQueue,"#ff8a3d");line("util",100,"#4aa3ff");ctx.font="11px system-ui";ctx.fillStyle="#ff8a3d";ctx.fillRect(p.l,h-15,8,3);ctx.fillStyle="#9fb3c8";ctx.fillText("cola",p.l+12,h-10);ctx.fillStyle="#4aa3ff";ctx.fillRect(p.l+53,h-15,8,3);ctx.fillStyle="#9fb3c8";ctx.fillText("utilización",p.l+65,h-10);ctx.fillText(maxQueue+" tareas",0,p.t+4);ctx.fillText("100%",0,h-p.b+4);}
    function renderDispatch(){const box=document.getElementById("dispatchSummary"),last=state.lastDispatch;if(!last){box.textContent="El Driver está esperando tareas para distribuir.";return;}box.innerHTML="El Driver envió la <strong>tarea T"+last.taskId+"</strong> del Job "+last.jobId+" al <strong>Worker "+last.workerId+"</strong>."; }
    function render(){renderMetrics();renderWorkers();renderJobs();renderDispatch();renderChart();}
    // Bucle: recuperación, progreso, nueva carga, fallos, planificación y renderizado.
    function tick(){if(!state.running)return;syncWorkers();recover();progress();autoJob();randomFailure();schedule();record();render();}
    function reset(){Object.assign(state,{running:true,jobId:0,taskId:0,completed:0,failures:0,totalLatency:0,jobsReceived:0,scheduled:0,autoClock:0,workers:[],queue:[],history:[],lastDispatch:null});syncWorkers();submitJob();schedule();record();document.getElementById("toggle").textContent="Pausar";render();}
    Object.keys(input).forEach(function(key){input[key].addEventListener("input",function(){updateLabels();syncWorkers();schedule();render();});});
    document.getElementById("addJob").addEventListener("click",function(){submitJob();schedule();render();});
    document.getElementById("toggle").addEventListener("click",function(){state.running=!state.running;this.textContent=state.running?"Pausar":"Reanudar";render();});
    document.getElementById("fail").addEventListener("click",function(){const candidates=state.workers.filter(function(w){return w.status==="up";});if(candidates.length)failWorker(candidates[random(0,candidates.length-1)]);schedule();render();});
    document.getElementById("reset").addEventListener("click",reset);window.addEventListener("resize",renderChart);
    updateLabels();reset();window.setInterval(tick,700);

