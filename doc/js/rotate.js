var	slides=null;
var	current=0;
var	opacity=1;

function rotate() {
	slides=document.getElementsByClassName("slide");
	if (slides && slides.length) {
		setTimeout("fadeOut()",2000);
	}
}

function fadeOut() {
	opacity=opacity-0.1;
	if (opacity<=0.1) {
		slides[current].style.display="none";
		current=current+1;
		if (current==slides.length) {
			current=0;
		}
		opacity=0;
		slides[current].style.opacity=0;
		slides[current].style.display="block";
		setTimeout("fadeIn()",35);
	} else {
		slides[current].style.opacity=opacity;
		setTimeout("fadeOut()",45);
	}
}

function fadeIn() {
	opacity=opacity+0.1;
	if (opacity>=1) {
		opacity=1;
		slides[current].style.opacity=1;
		setTimeout("fadeOut()",5000);
	} else {
		slides[current].style.opacity=opacity;
		setTimeout("fadeIn()",35);
	}
}
