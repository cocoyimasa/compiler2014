program test1;
const PI:=3.14159;
      E:=2.1111;
var i:integer;
	t1:integer;
	t2:float;
	k:integer;
	n:integer;
type anc=record 
   c:integer;
   zz:string;
   gfd:float;
   ghdg122:float;
   end;
function testFunc(a:integer;b:integer):integer;
var testVar:integer;
begin
a:=10;
b:=1000.22;
testFunc:=10;
end;
begin
  write("Hello World");
  i:=10;
  if i=10 then
  begin
    i:=11;
	t1:=3888;
	t2:=34.33;
	testFunc(200,300);
  end;
  if i>10 then
  begin
    i:=11;
	t1:=3888.2;
	t2:=34.33;
	n:=t1+10.1;
	write(n);
	write(t2);
  end;
  while i=10 do
  begin
    t1:=377;
	t2:=37337.376363;
  end;
  for k:=0 to 10 do
  begin
    i:=k;
    write(i);
  end;
end.