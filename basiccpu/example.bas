func main
	' blah
	if a = 0 then
		let a = 1
	end if

	' let b = 1
	if b = 1 then   'blah b lah
		let b = 2
		let a = 2
	end if

	' let a = a + b
	' let a = a + 1
	add a b
	add a 10

	print a
	die
end func


func blah
	let a = 1
	print a
end func