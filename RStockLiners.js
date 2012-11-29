//var TIME = new Array();
var OPEN = new Array();
var HIGH = new Array();
var LOW = new Array();
var CLOSE = new Array();
var VOLUME = new Array();
var AMOUNT = new Array();
var ADVANCE = new Array();
var DECLINE = new Array();
	
function CalcBaseData()
{
	OPEN.length = 0;
	HIGH.length = 0;
	LOW.length = 0;
	CLOSE.length = 0;
	VOLUME.length = 0;
	AMOUNT.length = 0;
	ADVANCE.length = 0;
	DECLINE.length = 0;
	
	var count = ITEMS.length;
	for(var i=0;i<count;++i)
	{
		OPEN[i] = ITEMS[i].open;
		HIGH[i] = ITEMS[i].high;
		LOW[i] = ITEMS[i].low;
		CLOSE[i] = ITEMS[i].close;
		VOLUME[i] = ITEMS[i].volume;
		AMOUNT[i] = ITEMS[i].amount;
		ADVANCE[i] = ITEMS[i].advance;
		DECLINE[i] = ITEMS[i].decline;
	}
	return "calc base data ok!";
}

function SUB(list, f)
{
	var count = list.length;
	var dest = new Array();
	for(var i=0;i<count;++i)
	{
		dest[i] = list[i]-f;
	}
	return dest;
}

function ADD(list, f)
{
	var count = list.length;
	var dest = new Array();
	for(var i=0;i<count;++i)
	{
		dest[i] = list[i]+f;
	}
	return dest;
}

function DIV(list, f)
{
	var count = list.length;
	var dest = new Array();
	for(var i=0;i<count;++i)
	{
		dest[i] = list[i]/f;
	}
	return dest;
}

function MUL(list, f)
{
	var count = list.length;
	var dest = new Array();
	for(var i=0;i<count;++i)
	{
		dest[i] = list[i]*f;
	}
	return dest;
}