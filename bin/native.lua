Array=
{
	__add=function(op1,op2)
		local op = {}
		local _t1 = type(op1)
		local _t2 = type(op2)
		
		if(_t2 == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					op[i] = op1[i]+op2[i]
				else
					op[i] = op1[i]
				end
			end
		elseif(_t2 == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]+op2
			end
		else
			op = op1+op2
		end
		
		setmetatable(op,Array)
		return op
	end,
	
	__sub=function(op1,op2)
		local op={}
		local _t1 = type(op1)
		local _t2 = type(op2)
		
		if(_t2 == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					op[i] = op1[i]-op2[i]
				else
					op[i] = 0
				end
			end
		elseif(_t2 == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]-op2
			end
		else
			op = op1-op2
		end
		setmetatable(op,Array)
		return op
	end,
	
	__mul=function(op1,op2)
		local op={}
		local _t1 = type(op1)
		local _t2 = type(op2)
		
		if(_t2 == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					op[i] = op1[i]*op2[i]
				else
					op[i] = 0
				end
			end
		elseif(_t2 == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]*op2
			end
		else
			op = op1*op2
		end
		setmetatable(op,Array)
		return op
	end,
	
	__div=function(op1,op2)
		local op={}
		local _t1 = type(op1)
		local _t2 = type(op2)
		
		if(_t2 == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					if(op2[i]==0) then
						op[i]=0
					else
						op[i] = op1[i]/op2[i]
					end
				else
					op[i] = 0
				end
			end
		elseif(_t2 == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]/op2
			end
		else
			op = op1/op2
		end
		setmetatable(op,Array)
		return op
	end,	
	
	create=function(o)
		o = o or {}
		setmetatable(o,Array)
		return o
	end
}

--[[
别名: 向前引用
所属类别: 引用函数  参数数量: 2

引用若干周期前的数据。
用法:
REF(X,A),引用A周期前的X值。
例如:
REF(CLOSE(),1)
表示上一周期的收盘价，在日线上就是昨收
]]
function REF(op1,op2)
	if op2<1 then
		return op1
	elseif type(op2)~="number" then
		return op1
	elseif type(op1)~="table" then
		return op1
	end
	--[[
	local op=1
	setmetatable(op,Array)
	return op]]
	
	local op={}
	local c = #(op1)
	for i=1,(c-op2) do
		op[i] = op1[i+op2]
	end
	local c1 = #(op)
	for i=(c1+1),c do
		op[i] = op1[c]
	end
	setmetatable(op,Array)
	return op
end

--[[
别名: 向后引用
所属类别: 引用函数  参数数量: 2

引用若干周期后的数据。
用法:
REFX(X,A),引用A周期后的X值。
例如:
REFX(CLOSE(),1)
表示后一周期的收盘价，在日线上就是明收
]]
function REFX(op1,op2)
	if(op2<1 or type(op2)~="number" or type(op1)~="table") then
		return op1
	end
	
	local c = #(op1)
	if(c<op2) then
		return op1
	end
	
	local op={}
	for i=1,op2 do
		op[i] = op1[1]
	end
	
	for i=1,(c-op2) do
		op[i+op2] = op1[i]
	end
	
	setmetatable(op,Array)
	return op
end

--[[
别名: 简单移动平均
所属类别: 引用函数  参数数量: 2

求简单移动平均。
用法:
MA(X,N),求X的N周期简单移动平均值。
算法：
(X1+X2+X3+...+Xn)/N
例如:
MA(CLOSE,20)
表示求20日均价
]]
function MA(op1,op2)
	if(type(op1)~="table" or type(op2)~="number") then
		return op1;
	end
	
	local op={}
	local c = #(op1)
	for i=1,c do
		local sum = 0;
		local j = i;
		local step = 0;
		while (j>=1 and step<op2) do
			sum=sum+op1[j];
			j=j-1;
			step=step+1;
		end
		op[i]=sum/step;
	end
	return op;
end


function InitValues()
	CLOSE=RClose();
	OPEN=ROpen();
	HIGH=RHigh();
	LOW=RLow();
	VOLUME=RVolume();
end

--a={1.2,23.3,4}
--a=Array.create(a)
--b={1,2}
--c = a + b
--print(table.concat(c,"\t"))
--d=c*b
--print(table.concat(d,"\t"))

--print(#(op1))		-- no
