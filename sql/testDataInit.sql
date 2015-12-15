insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('张三', 'a', 2, 1, '0', '', 0);
insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('李四', 'b', 1, 1, '0', '', 0);
insert into usr_info(usr_name, usr_password, usr_friend_num, usr_group_num, usr_status, usr_local_ip, usr_local_port) 
values ('王五', 'c', 1, 1, '0', '', 0);

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
values ('桌球群', 3, 0);
