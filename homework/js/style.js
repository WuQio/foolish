$(function(){
			var width = $(window).width();
			// Initial width and height.
			var Size = function(){
				var W = $(window).width();
				$('li img').width(W);
				$('li img').height(550/1340*W);
				// console.log($(window).width());
				var imgHeight = $('li img').height(),
					imgWidth = $('li img').width();
				$('.carousel li').width(imgWidth);
				$('.wrap').width(imgWidth);
				$('.wrap').height(imgHeight);
				$('.carousel').width(imgWidth*5);
				$('.carousel').height(imgHeight);
				$('.top').width(W);
				$('.top').height(70/1366*W);
				// console.log($(window).width());
				$('.toplogo').width(114/1366*W);
				$('.toplogo').height(107/114*$('.logo').width());
				$('.toplogo').css("left", 0.15*W)
				$('.links').css({"right":120/1366*W, "top":8/1366*W});
				if($(window).width() < 992)
				{
					$('.link1').hide();
				}
				else
				{
					$('.link1').show();
				}
				$('.dropdown-row>li').width(120/1366*$(window).width());
				$('.dropdown-row>li').css("font-size", 16/1366*$(window).width()+"px");
				$('.dropdown-row>li').css("line-height", 40/1366*$(window).width()+"px");
			}
			Size();
			// Change width and height when window's change.
			$(window).resize(function(){
				//Maybe function resize is allowed to use in jQuery.
				Size();
				var left = $('.carousel').css("left");
				if($(window).width() < width)
				{// Window zoom out. Right move
					if(i%5)
						$('.carousel').css("left", parseInt(left)+(width-$(window).width())*(i%5-1)+"px");
					else
						$('.carousel').css("left", parseInt(left)+(width-$(window).width())*(4)+"px");
				}
				else
				{// Zoom in. Left move
					if(i%5)
						$('.carousel').css("left", parseInt(left)-($(window).width()-width)*(i%5-1)+"px");
					else
						$('.carousel').css("left", parseInt(left)-($(window).width()-width)*(4)+"px");
				}
				width = $(window).width();
			});
			var i = 1; // Initial i can't be zero
			var carousel = function(){
				$('.carousel').animate({left:-$('li img').width()*(i%5)+"px"});
				$('.dots').children().removeClass('active');
				$('.dots').children().eq(i%5).addClass('active');
				i++;
			};
			setInterval(carousel, 3000);
			$('.dots a').click(function(){
				i = $('.dots a').index(this);
				$('.carousel').animate({left:-$('li img').width()*(i%5)+"px"});
				$('.dots').children().removeClass('active');
				$('.dots').children().eq(i%5).addClass('active');
				i++;
			});

			$('.dropdown-row>li').mouseover(function(){
				$(this).children('.dropdown').show();
				$(this).css("color", "white");
			});
			$('.dropdown-row>li').mouseout(function(){
				$(this).children('.dropdown').hide();
				$(this).css("color", "black");
			});
			$('.dropdown-items').mouseover(function(){$(this).css("background","#FDD335");});
			$('.dropdown-items').mouseout(function(){$(this).css("background","#333333");});
		});