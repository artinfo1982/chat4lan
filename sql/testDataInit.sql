insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('a', 'a', 2, 1, 1, '192.168.3.12', 60000);
insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('b', 'b', 1, 1, 1, '192.168.3.12', 50000);
insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('c', 'c', 1, 1, 1, '192.168.3.12', 40000);

insert into usr_friend_info(usr_id, usr_friend_id) 
values (1, 2);
insert into usr_friend_info(usr_id, usr_friend_id) 
values (1, 3);
insert into usr_friend_info(usr_id, usr_friend_id) 
values (2, 1);
insert into usr_friend_info(usr_id, usr_friend_id) 
values (3, 1);

insert into usr_group_info(usr_id, group_id) 
values (1, 1);
insert into usr_group_info(usr_id, group_id) 
values (2, 1);
insert into usr_group_info(usr_id, group_id) 
values (3, 1);

insert into group_info(group_name, group_member_num, group_online_member_num) 
values ('A', 3, 3);

insert into group_member_info(group_id, member_id) 
values (1, 1);
insert into group_member_info(group_id, member_id) 
values (1, 2);
insert into group_member_info(group_id, member_id) 
values (1, 3);
