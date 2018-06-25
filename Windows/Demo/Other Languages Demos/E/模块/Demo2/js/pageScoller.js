(function(){
	pageScoller=function(id,cfg){
		cfg.child=cfg.child||"page";
		cfg.endClass=cfg.endClass||"show";
		var $pages=$("#"+id).find("."+cfg.child);
		var data={
			pLength:$pages.length,
			curP:0,
			isCan:true
		};
		var pageChange=function(idx){
			if(idx>=0&&idx<data.pLength&&idx!=data.curP&&data.isCan){
				var ct,nt;
				if(idx>data.curP)
					ct="-100%",nt="100%";
				else
					ct="100%",nt="-100%";
				data.isCan=false;
				$pages.eq(data.curP).stop().animate({top:ct},400);
				if(cfg.bCallback&&typeof(cfg.bCallback)=="function")
					cfg.bCallback($pages,idx);
				$pages.eq(idx).css("top",nt).animate({top:0},400,function(){
					data.isCan=true;
					$(this).addClass(cfg.endClass);
					$pages.eq(data.curP).removeClass(cfg.endClass);
					data.curP=idx;
					if(cfg.aCallback&&typeof(cfg.aCallback)=="function")
						cfg.aCallback($pages,idx);
				});
			}
		};
		var changePrev=function(){
			pageChange((data.curP-1)%data.pLength);
		};
		var changeNext=function(){
			pageChange((data.curP+1)%data.pLength);
		}
		$(document).on("mousewheel DOMMouseScroll",function(event){
			var sd=event.originalEvent.wheelDelta||event.originalEvent.detail*-1;
			if(sd>0)
				changePrev();
			else
				changeNext();
		});
		return {
				change: pageChange,
				prev:   changePrev,
				next:   changeNext
		};
	}
})()