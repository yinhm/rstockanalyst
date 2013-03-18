Array=
{
	__add=function(op1,op2)
		op = {}
		if(type(op2) == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					op[i] = op1[i]+op2[i]
				else
					op[i] = op1[i]
				end
			end
		elseif(type(op2) == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]+op2
			end
		else
			op = op1+op2
		end
		
		setmetatable(op,Array)
		return op
	end,
	
	__mul=function(op1,op2)
		op={}
		if(type(op2) == "table") then
			local c = #(op2)
			for i=1, #(op1) do
				if(i<=c) then
					op[i] = op1[i]*op2[i]
				else
					op[i] = 0
				end
			end
		elseif(type(op2) == "number") then
			for i=1, #(op1) do
				op[i] = op1[i]*op2
			end
		else
			op = op1+op2
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

a={1.2,23.3,4}
a=Array.create(a)
b={1,2}
c = a + b
print(table.concat(c,"\t"))
d=c*b
print(table.concat(d,"\t"))