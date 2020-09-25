function dump(value, nesting, trace)
	local function dumpValue(v)
		if type(v) == "string" then
			return "\""..v.."\""
		end
		return tostring(v)
	end

	if trace then
		local traceback = string.split(debug.traceback("", 2), "\n")
		local from = traceback[2]
		if from then
			print(from)
		end
	end

	if type(nesting) ~= "number" then nesting = 3 end

	local lines = {}
	local lookups = {}

	local function dumpInternal(key, value, indent, nest, keylen)
		local spc = string.rep(" ", keylen-string.len(key))
		if type(value) ~= "table" then
			if key == "" then
				table.insert(lines, indent..dumpValue(value))
			else
				table.insert(lines, indent..key..spc.." = "..dumpValue(value))
			end
		elseif lookups[tostring(value)] then
			table.insert(lines, indent..key..spc.." = *REF*")
		else
			lookups[tostring(value)] = true
			if nest>nesting then
				table.insert(lines, indent..key..spc.." = *MAX NESTING*")
			else
				local ks = {}
				local kl = 0
				for k,v in pairs(value) do
					local l = string.len(k)
					if l > kl then
						kl = l
					end
					table.insert(ks, k)
				end
				table.sort(ks, function(a, b)
					if type(a)=="number" and type(b)=="number" then
						return a < b
					else
						return tostring(a) < tostring(b)
					end
				end)
				if key == "" then
					table.insert(lines, indent.."{")
				else
					table.insert(lines, indent..key.." = {")
				end
				for _,k in ipairs(ks) do
					dumpInternal(k, value[k], indent.."  ", nest+1, kl)
				end
				table.insert(lines, indent.."}")
			end
		end
	end

	dumpInternal("", value, "- ", 1, 0)

	for _,line in ipairs(lines) do
		print(line)
	end
end