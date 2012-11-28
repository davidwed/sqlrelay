create package "serialpkg" as
	procedure set_last_serial(lsv in number);
	procedure get_last_serial(lsv out number);
end serialpkg;
/

create package body "serialpkg" as
	last_serial number(10):=0;
	procedure set_last_serial(lsv in number) is
	begin
		last_serial:=lsv;
	end set_last_serial;
	procedure get_last_serial(lsv out number) is
	begin
		lsv:=last_serial;
	end get_last_serial;
 end serialpkg;
/
