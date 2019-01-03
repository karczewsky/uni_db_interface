te schema public;

comment on schema public is 'standard public schema';

alter schema public owner to postgres;

create table if not exists teacher
(
	id serial not null
		constraint employee_pk
			primary key,
	first_name varchar(40) not null,
	last_name varchar(50) not null,
	city varchar(30) not null,
	street varchar(50) not null,
	postal_code varchar(6) not null,
	email varchar(50) not null
);

alter table teacher owner to postgres;

create table if not exists module
(
	id serial not null
		constraint module_pk
			primary key,
	name varchar(40) not null,
	ects integer not null
);

alter table module owner to postgres;

create table if not exists course_group
(
	id serial not null
		constraint course_group_pk
			primary key,
	name varchar(10) not null
		constraint course_group_un
			unique
);

alter table course_group owner to postgres;

create table if not exists student
(
	album_no serial not null
		constraint student_pk
			primary key,
	first_name varchar(40) not null,
	last_name varchar(50) not null,
	city varchar(30) not null,
	street varchar(50) not null,
	postal_code varchar(6) not null,
	email varchar(50) not null,
	group_id integer not null
		constraint student_course_group_id_fk
			references course_group
				on delete cascade
);

alter table student owner to postgres;

create unique index if not exists course_group_name_uindex
	on course_group (name);

create table if not exists qualified
(
	teacher_id integer not null
		constraint qualified_teacher_id_fk
			references teacher
				on delete cascade,
	module_id integer not null
		constraint qualified_module_id_fk
			references module
				on delete cascade,
	constraint qualified_pk
		primary key (teacher_id, module_id)
);

alter table qualified owner to postgres;

create table if not exists group_module
(
	group_id integer not null
		constraint group_module_course_group_id_fk
			references course_group
				on delete cascade,
	teacher_id integer not null,
	module_id integer not null,
	constraint group_module_pk
		primary key (group_id, teacher_id, module_id),
	constraint group_module_qualified_teacher_id_module_id_fk
		foreign key (teacher_id, module_id) references qualified
			on delete cascade
);

alter table group_module owner to postgres;

create table if not exists grade
(
	student_id integer not null
		constraint grade_student_album_no_fk
			references student
				on delete cascade,
	teacher_id integer not null
		constraint grade_teacher_id_fk
			references teacher
				on delete cascade,
	module_id integer not null
		constraint grade_module_id_fk
			references module
				on delete cascade,
	mark numeric(2,1),
	constraint grade_pk
		primary key (student_id, teacher_id, module_id)
);

alter table grade owner to postgres;


